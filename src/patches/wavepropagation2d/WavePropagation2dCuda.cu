/**
 * @section DESCRIPTION
 * CUDA-backed two-dimensional F-Wave propagation patch.
 **/
#include "WavePropagation2dCuda.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
constexpr tsunami_lab::t_real g = 9.80665f;
constexpr tsunami_lab::t_real gSqrt = 3.131557121f;

void checkCuda(cudaError_t i_error, char const * i_operation) {
  if (i_error != cudaSuccess) {
    throw std::runtime_error(std::string("CUDA error in ") + i_operation + ": "
                           + cudaGetErrorString(i_error));
  }
}

__device__ void fwave(tsunami_lab::t_real hL,
                      tsunami_lab::t_real hR,
                      tsunami_lab::t_real huL,
                      tsunami_lab::t_real huR,
                      tsunami_lab::t_real bL,
                      tsunami_lab::t_real bR,
                      tsunami_lab::t_real & minusH,
                      tsunami_lab::t_real & minusHu,
                      tsunami_lab::t_real & plusH,
                      tsunami_lab::t_real & plusHu) {
  minusH = minusHu = plusH = plusHu = 0.0f;
  bool const leftDry = hL <= 0.0f;
  bool const rightDry = hR <= 0.0f;
  if (leftDry) {
    if (rightDry) return;
    hL = hR;
    huL = -huR;
    bL = bR;
  }
  else if (rightDry) {
    hR = hL;
    huR = -huL;
    bR = bL;
  }

  tsunami_lab::t_real const sqrtHL = sqrtf(hL);
  tsunami_lab::t_real const sqrtHR = sqrtf(hR);
  tsunami_lab::t_real const hRoe = 0.5f * (hL + hR);
  tsunami_lab::t_real uRoe = sqrtHL * (huL / hL) + sqrtHR * (huR / hR);
  uRoe /= sqrtHL + sqrtHR;
  tsunami_lab::t_real const speedOffset = gSqrt * sqrtf(hRoe);
  tsunami_lab::t_real const speedL = uRoe - speedOffset;
  tsunami_lab::t_real const speedR = uRoe + speedOffset;

  tsunami_lab::t_real const totalL = huL * huL / hL + 0.5f * (g * hL * hL);
  tsunami_lab::t_real const totalR = huR * huR / hR + 0.5f * (g * hR * hR);
  tsunami_lab::t_real const deltaF0 = huR - huL;
  tsunami_lab::t_real deltaF1 = totalR - totalL;
  deltaF1 -= (-g) * (bR - bL) * ((hL + hR) / 2.0f);
  tsunami_lab::t_real const inverseDeterminant = 1.0f / (speedR - speedL);
  tsunami_lab::t_real const alphaL = (speedR * inverseDeterminant) * deltaF0
                                   + (-inverseDeterminant) * deltaF1;
  tsunami_lab::t_real const alphaR = (-speedL * inverseDeterminant) * deltaF0
                                   + inverseDeterminant * deltaF1;

  if (speedL < 0.0f) {
    minusH += alphaL;
    minusHu += alphaL * speedL;
  }
  else {
    plusH += alphaL;
    plusHu += alphaL * speedL;
  }

  // Keep behavior identical to the current production FWave implementation.
  if (speedR < 0.0f) {
    minusH += minusH + alphaR;
    minusHu += minusHu + alphaR * speedR;
  }
  else {
    plusH += plusH + alphaR;
    plusHu += plusHu + alphaR * speedR;
  }

  if (leftDry) {
    minusH = minusHu = 0.0f;
  }
  else if (rightDry) {
    plusH = plusHu = 0.0f;
  }
}

__global__ void copyState(tsunami_lab::t_real const * hOld,
                          tsunami_lab::t_real const * huOld,
                          tsunami_lab::t_real const * hvOld,
                          tsunami_lab::t_real * hNew,
                          tsunami_lab::t_real * huNew,
                          tsunami_lab::t_real * hvNew,
                          std::size_t cells) {
  std::size_t const id = blockIdx.x * blockDim.x + threadIdx.x;
  if (id < cells) {
    hNew[id] = hOld[id];
    huNew[id] = huOld[id];
    hvNew[id] = hvOld[id];
  }
}

__global__ void xEdges(tsunami_lab::t_real const * h,
                       tsunami_lab::t_real const * hu,
                       tsunami_lab::t_real const * b,
                       tsunami_lab::t_real * minusH,
                       tsunami_lab::t_real * minusHu,
                       tsunami_lab::t_real * plusH,
                       tsunami_lab::t_real * plusHu,
                       int xCells,
                       int yCells) {
  std::size_t const edge = blockIdx.x * blockDim.x + threadIdx.x;
  std::size_t const count = static_cast<std::size_t>(xCells + 1) * yCells;
  if (edge >= count) return;
  int const ex = static_cast<int>(edge % (xCells + 1));
  int const ey = static_cast<int>(edge / (xCells + 1)) + 1;
  int const stride = xCells + 2;
  std::size_t const left = static_cast<std::size_t>(ey) * stride + ex;
  std::size_t const right = left + 1;
  fwave(h[left], h[right], hu[left], hu[right], b[left], b[right],
        minusH[edge], minusHu[edge], plusH[edge], plusHu[edge]);
}

__global__ void applyX(tsunami_lab::t_real scale,
                       tsunami_lab::t_real * h,
                       tsunami_lab::t_real * hu,
                       tsunami_lab::t_real const * minusH,
                       tsunami_lab::t_real const * minusHu,
                       tsunami_lab::t_real const * plusH,
                       tsunami_lab::t_real const * plusHu,
                       int xCells,
                       int yCells) {
  std::size_t const cell = blockIdx.x * blockDim.x + threadIdx.x;
  std::size_t const count = static_cast<std::size_t>(xCells) * yCells;
  if (cell >= count) return;
  int const x = static_cast<int>(cell % xCells) + 1;
  int const y = static_cast<int>(cell / xCells) + 1;
  std::size_t const leftEdge = static_cast<std::size_t>(y - 1) * (xCells + 1) + (x - 1);
  std::size_t const rightEdge = leftEdge + 1;
  std::size_t const id = static_cast<std::size_t>(y) * (xCells + 2) + x;
  h[id] -= scale * (plusH[leftEdge] + minusH[rightEdge]);
  hu[id] -= scale * (plusHu[leftEdge] + minusHu[rightEdge]);
}

__global__ void yEdges(tsunami_lab::t_real const * h,
                       tsunami_lab::t_real const * hv,
                       tsunami_lab::t_real const * b,
                       tsunami_lab::t_real * minusH,
                       tsunami_lab::t_real * minusHv,
                       tsunami_lab::t_real * plusH,
                       tsunami_lab::t_real * plusHv,
                       int xCells,
                       int yCells) {
  std::size_t const edge = blockIdx.x * blockDim.x + threadIdx.x;
  std::size_t const count = static_cast<std::size_t>(xCells) * (yCells + 1);
  if (edge >= count) return;
  int const x = static_cast<int>(edge % xCells) + 1;
  int const ey = static_cast<int>(edge / xCells);
  int const stride = xCells + 2;
  std::size_t const bottom = static_cast<std::size_t>(ey) * stride + x;
  std::size_t const top = bottom + stride;
  fwave(h[bottom], h[top], hv[bottom], hv[top], b[bottom], b[top],
        minusH[edge], minusHv[edge], plusH[edge], plusHv[edge]);
}

__global__ void applyY(tsunami_lab::t_real scale,
                       tsunami_lab::t_real * h,
                       tsunami_lab::t_real * hv,
                       tsunami_lab::t_real const * minusH,
                       tsunami_lab::t_real const * minusHv,
                       tsunami_lab::t_real const * plusH,
                       tsunami_lab::t_real const * plusHv,
                       int xCells,
                       int yCells) {
  std::size_t const cell = blockIdx.x * blockDim.x + threadIdx.x;
  std::size_t const count = static_cast<std::size_t>(xCells) * yCells;
  if (cell >= count) return;
  int const x = static_cast<int>(cell % xCells) + 1;
  int const y = static_cast<int>(cell / xCells) + 1;
  std::size_t const bottomEdge = static_cast<std::size_t>(y - 1) * xCells + (x - 1);
  std::size_t const topEdge = bottomEdge + xCells;
  std::size_t const id = static_cast<std::size_t>(y) * (xCells + 2) + x;
  h[id] -= scale * (plusH[bottomEdge] + minusH[topEdge]);
  hv[id] -= scale * (plusHv[bottomEdge] + minusHv[topEdge]);
}

__global__ void ghostColumns(tsunami_lab::t_real * h,
                             tsunami_lab::t_real * hu,
                             tsunami_lab::t_real * b,
                             int xCells,
                             int yCells,
                             bool reflecting) {
  int const y = blockIdx.x * blockDim.x + threadIdx.x + 1;
  if (y > yCells) return;
  int const stride = xCells + 2;
  std::size_t const leftGhost = static_cast<std::size_t>(y) * stride;
  std::size_t const rightGhost = leftGhost + xCells + 1;
  h[leftGhost] = h[leftGhost + 1];
  h[rightGhost] = h[rightGhost - 1];
  b[leftGhost] = b[leftGhost + 1];
  b[rightGhost] = b[rightGhost - 1];
  tsunami_lab::t_real const sign = reflecting ? -1.0f : 1.0f;
  hu[leftGhost] = sign * hu[leftGhost + 1];
  hu[rightGhost] = sign * hu[rightGhost - 1];
}

__global__ void ghostRows(tsunami_lab::t_real * h,
                          tsunami_lab::t_real * hv,
                          tsunami_lab::t_real * b,
                          int xCells,
                          int yCells,
                          bool reflecting) {
  int const x = blockIdx.x * blockDim.x + threadIdx.x + 1;
  if (x > xCells) return;
  int const stride = xCells + 2;
  std::size_t const bottomGhost = x;
  std::size_t const topGhost = static_cast<std::size_t>(yCells + 1) * stride + x;
  h[bottomGhost] = h[bottomGhost + stride];
  h[topGhost] = h[topGhost - stride];
  b[bottomGhost] = b[bottomGhost + stride];
  b[topGhost] = b[topGhost - stride];
  tsunami_lab::t_real const sign = reflecting ? -1.0f : 1.0f;
  hv[bottomGhost] = sign * hv[bottomGhost + stride];
  hv[topGhost] = sign * hv[topGhost - stride];
}
}

tsunami_lab::patches::WavePropagation2dCuda::WavePropagation2dCuda(t_idx i_xCells,
                                                                   t_idx i_yCells,
                                                                   bool  i_useRoe,
                                                                   bool  i_reflectingBoundary):
  m_xCells(i_xCells),
  m_yCells(i_yCells),
  m_cells((i_xCells + 2) * (i_yCells + 2)),
  m_maxEdges(std::max((i_xCells + 1) * i_yCells, i_xCells * (i_yCells + 1))),
  m_reflecting(i_reflectingBoundary) {
  if (i_useRoe) {
    throw std::runtime_error("CUDA WavePropagation2d backend currently supports only the F-Wave solver.");
  }

  for (unsigned short l_st = 0; l_st < 2; ++l_st) {
    m_hostH[l_st] = new t_real[m_cells]();
    m_hostHu[l_st] = new t_real[m_cells]();
    m_hostHv[l_st] = new t_real[m_cells]();
    checkCuda(cudaMalloc(&m_deviceH[l_st], m_cells * sizeof(t_real)), "cudaMalloc h");
    checkCuda(cudaMalloc(&m_deviceHu[l_st], m_cells * sizeof(t_real)), "cudaMalloc hu");
    checkCuda(cudaMalloc(&m_deviceHv[l_st], m_cells * sizeof(t_real)), "cudaMalloc hv");
  }
  m_hostB = new t_real[m_cells]();
  checkCuda(cudaMalloc(&m_deviceB, m_cells * sizeof(t_real)), "cudaMalloc b");
  for (t_real *& l_edge : m_edges) {
    checkCuda(cudaMalloc(&l_edge, m_maxEdges * sizeof(t_real)), "cudaMalloc edge");
  }
}

tsunami_lab::patches::WavePropagation2dCuda::~WavePropagation2dCuda() {
  for (unsigned short l_st = 0; l_st < 2; ++l_st) {
    delete[] m_hostH[l_st];
    delete[] m_hostHu[l_st];
    delete[] m_hostHv[l_st];
    cudaFree(m_deviceH[l_st]);
    cudaFree(m_deviceHu[l_st]);
    cudaFree(m_deviceHv[l_st]);
  }
  delete[] m_hostB;
  cudaFree(m_deviceB);
  for (t_real * l_edge : m_edges) {
    cudaFree(l_edge);
  }
}

void tsunami_lab::patches::WavePropagation2dCuda::uploadIfNeeded() {
  if (!m_hostDirty) return;
  std::size_t const l_bytes = m_cells * sizeof(t_real);
  checkCuda(cudaMemcpy(m_deviceH[m_step], m_hostH[m_step], l_bytes, cudaMemcpyHostToDevice), "upload h");
  checkCuda(cudaMemcpy(m_deviceHu[m_step], m_hostHu[m_step], l_bytes, cudaMemcpyHostToDevice), "upload hu");
  checkCuda(cudaMemcpy(m_deviceHv[m_step], m_hostHv[m_step], l_bytes, cudaMemcpyHostToDevice), "upload hv");
  checkCuda(cudaMemcpy(m_deviceB, m_hostB, l_bytes, cudaMemcpyHostToDevice), "upload b");
  m_hostDirty = false;
}

void tsunami_lab::patches::WavePropagation2dCuda::downloadIfNeeded() {
  if (!m_deviceDirty) return;
  std::size_t const l_bytes = m_cells * sizeof(t_real);
  checkCuda(cudaMemcpy(m_hostH[m_step], m_deviceH[m_step], l_bytes, cudaMemcpyDeviceToHost), "download h");
  checkCuda(cudaMemcpy(m_hostHu[m_step], m_deviceHu[m_step], l_bytes, cudaMemcpyDeviceToHost), "download hu");
  checkCuda(cudaMemcpy(m_hostHv[m_step], m_deviceHv[m_step], l_bytes, cudaMemcpyDeviceToHost), "download hv");
  checkCuda(cudaMemcpy(m_hostB, m_deviceB, l_bytes, cudaMemcpyDeviceToHost), "download b");
  m_deviceDirty = false;
}

void tsunami_lab::patches::WavePropagation2dCuda::setGhostCellsOnHost() {
  t_real * l_h = m_hostH[m_step];
  t_real * l_hu = m_hostHu[m_step];
  t_real * l_hv = m_hostHv[m_step];

  for (t_idx l_g = 1; l_g < m_yCells + 1; ++l_g) {
    l_h[getIndex(0, l_g)] = l_h[getIndex(1, l_g)];
    l_h[getIndex(m_xCells + 1, l_g)] = l_h[getIndex(m_xCells, l_g)];
    m_hostB[getIndex(0, l_g)] = m_hostB[getIndex(1, l_g)];
    m_hostB[getIndex(m_xCells + 1, l_g)] = m_hostB[getIndex(m_xCells, l_g)];
    t_real const l_sign = m_reflecting ? -1.0f : 1.0f;
    l_hu[getIndex(0, l_g)] = l_sign * l_hu[getIndex(1, l_g)];
    l_hu[getIndex(m_xCells + 1, l_g)] = l_sign * l_hu[getIndex(m_xCells, l_g)];
  }

  for (t_idx l_g = 1; l_g < m_xCells + 1; ++l_g) {
    l_h[getIndex(l_g, 0)] = l_h[getIndex(l_g, 1)];
    l_h[getIndex(l_g, m_yCells + 1)] = l_h[getIndex(l_g, m_yCells)];
    m_hostB[getIndex(l_g, 0)] = m_hostB[getIndex(l_g, 1)];
    m_hostB[getIndex(l_g, m_yCells + 1)] = m_hostB[getIndex(l_g, m_yCells)];
    t_real const l_sign = m_reflecting ? -1.0f : 1.0f;
    l_hv[getIndex(l_g, 0)] = l_sign * l_hv[getIndex(l_g, 1)];
    l_hv[getIndex(l_g, m_yCells + 1)] = l_sign * l_hv[getIndex(l_g, m_yCells)];
  }
}

void tsunami_lab::patches::WavePropagation2dCuda::setGhostCellsOnDevice() {
  int constexpr l_block = 256;
  ghostColumns<<<static_cast<int>((m_yCells + l_block - 1) / l_block), l_block>>>(
    m_deviceH[m_step], m_deviceHu[m_step], m_deviceB,
    static_cast<int>(m_xCells), static_cast<int>(m_yCells), m_reflecting);
  ghostRows<<<static_cast<int>((m_xCells + l_block - 1) / l_block), l_block>>>(
    m_deviceH[m_step], m_deviceHv[m_step], m_deviceB,
    static_cast<int>(m_xCells), static_cast<int>(m_yCells), m_reflecting);
  checkCuda(cudaGetLastError(), "ghost cell kernels");
}

void tsunami_lab::patches::WavePropagation2dCuda::setGhostCollumn() {
  if (m_deviceDirty && !m_hostDirty) {
    setGhostCellsOnDevice();
    return;
  }
  downloadIfNeeded();
  setGhostCellsOnHost();
  m_hostDirty = true;
}

void tsunami_lab::patches::WavePropagation2dCuda::timeStep(t_real i_scaling) {
  uploadIfNeeded();

  int constexpr l_block = 256;
  int const l_cellBlocks = static_cast<int>((m_cells + l_block - 1) / l_block);
  std::size_t const l_interior = m_xCells * m_yCells;

  m_step = 1 - m_step;
  copyState<<<l_cellBlocks, l_block>>>(m_deviceH[1 - m_step],
                                       m_deviceHu[1 - m_step],
                                       m_deviceHv[1 - m_step],
                                       m_deviceH[m_step],
                                       m_deviceHu[m_step],
                                       m_deviceHv[m_step],
                                       m_cells);
  std::size_t const l_xCount = (m_xCells + 1) * m_yCells;
  xEdges<<<static_cast<int>((l_xCount + l_block - 1) / l_block), l_block>>>(
    m_deviceH[1 - m_step], m_deviceHu[1 - m_step], m_deviceB,
    m_edges[0], m_edges[1], m_edges[2], m_edges[3],
    static_cast<int>(m_xCells), static_cast<int>(m_yCells));
  applyX<<<static_cast<int>((l_interior + l_block - 1) / l_block), l_block>>>(
    i_scaling, m_deviceH[m_step], m_deviceHu[m_step],
    m_edges[0], m_edges[1], m_edges[2], m_edges[3],
    static_cast<int>(m_xCells), static_cast<int>(m_yCells));
  setGhostCellsOnDevice();

  m_step = 1 - m_step;
  copyState<<<l_cellBlocks, l_block>>>(m_deviceH[1 - m_step],
                                       m_deviceHu[1 - m_step],
                                       m_deviceHv[1 - m_step],
                                       m_deviceH[m_step],
                                       m_deviceHu[m_step],
                                       m_deviceHv[m_step],
                                       m_cells);
  std::size_t const l_yCount = m_xCells * (m_yCells + 1);
  yEdges<<<static_cast<int>((l_yCount + l_block - 1) / l_block), l_block>>>(
    m_deviceH[1 - m_step], m_deviceHv[1 - m_step], m_deviceB,
    m_edges[0], m_edges[1], m_edges[2], m_edges[3],
    static_cast<int>(m_xCells), static_cast<int>(m_yCells));
  applyY<<<static_cast<int>((l_interior + l_block - 1) / l_block), l_block>>>(
    i_scaling, m_deviceH[m_step], m_deviceHv[m_step],
    m_edges[0], m_edges[1], m_edges[2], m_edges[3],
    static_cast<int>(m_xCells), static_cast<int>(m_yCells));
  setGhostCellsOnDevice();

  checkCuda(cudaDeviceSynchronize(), "WavePropagation2dCuda::timeStep");
  m_deviceDirty = true;
  m_hostDirty = false;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2dCuda::getHeight() {
  downloadIfNeeded();
  return m_hostH[m_step];
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2dCuda::getBathymetry() {
  downloadIfNeeded();
  return m_hostB;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2dCuda::getMomentumX() {
  downloadIfNeeded();
  return m_hostHu[m_step];
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2dCuda::getMomentumY() {
  downloadIfNeeded();
  return m_hostHv[m_step];
}

void tsunami_lab::patches::WavePropagation2dCuda::setHeight(t_idx i_ix,
                                                           t_idx i_iy,
                                                           t_real i_h) {
  downloadIfNeeded();
  m_hostH[m_step][getIndex(i_ix + 1, i_iy + 1)] = i_h;
  m_hostDirty = true;
}

void tsunami_lab::patches::WavePropagation2dCuda::setBathymetry(t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_b) {
  downloadIfNeeded();
  m_hostB[getIndex(i_ix + 1, i_iy + 1)] = i_b;
  m_hostDirty = true;
}

void tsunami_lab::patches::WavePropagation2dCuda::setMomentumX(t_idx i_ix,
                                                              t_idx i_iy,
                                                              t_real i_hu) {
  downloadIfNeeded();
  m_hostHu[m_step][getIndex(i_ix + 1, i_iy + 1)] = i_hu;
  m_hostDirty = true;
}

void tsunami_lab::patches::WavePropagation2dCuda::setMomentumY(t_idx i_ix,
                                                              t_idx i_iy,
                                                              t_real i_hv) {
  downloadIfNeeded();
  m_hostHv[m_step][getIndex(i_ix + 1, i_iy + 1)] = i_hv;
  m_hostDirty = true;
}

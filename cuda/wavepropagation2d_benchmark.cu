/**
 * CUDA prototype of a complete F-Wave WavePropagation2d time step.
 *
 * Device arrays stay resident across time steps. Each directional sweep uses
 * one edge kernel and one conflict-free cell kernel, followed by ghost cells.
 */
#include <cuda_runtime.h>
#include "../src/patches/wavepropagation2d/WavePropagation2d.h"

#include <omp.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {
constexpr float g = 9.80665f;
constexpr float gSqrt = 3.131557121f;

void checkCuda(cudaError_t error, char const * operation) {
  if (error != cudaSuccess) {
    std::cerr << "CUDA error in " << operation << ": " << cudaGetErrorString(error) << '\n';
    std::exit(EXIT_FAILURE);
  }
}

__device__ void fwave(float hL, float hR, float huL, float huR, float bL, float bR,
                      float & minusH, float & minusHu, float & plusH, float & plusHu) {
  minusH = minusHu = plusH = plusHu = 0.0f;
  bool const leftDry = hL <= 0.0f;
  bool const rightDry = hR <= 0.0f;
  if (leftDry) {
    if (rightDry) return;
    hL = hR; huL = -huR; bL = bR;
  }
  else if (rightDry) {
    hR = hL; huR = -huL; bR = bL;
  }

  float const sqrtHL = sqrtf(hL);
  float const sqrtHR = sqrtf(hR);
  float const hRoe = 0.5f * (hL + hR);
  float uRoe = sqrtHL * (huL / hL) + sqrtHR * (huR / hR);
  uRoe /= sqrtHL + sqrtHR;
  float const speedOffset = gSqrt * sqrtf(hRoe);
  float const speedL = uRoe - speedOffset;
  float const speedR = uRoe + speedOffset;

  float const totalL = huL * huL / hL + 0.5f * (g * hL * hL);
  float const totalR = huR * huR / hR + 0.5f * (g * hR * hR);
  float const deltaF0 = huR - huL;
  float deltaF1 = totalR - totalL;
  deltaF1 -= (-g) * (bR - bL) * ((hL + hR) / 2.0f);
  float const inverseDeterminant = 1.0f / (speedR - speedL);
  float const alphaL = (speedR * inverseDeterminant) * deltaF0
                     + (-inverseDeterminant) * deltaF1;
  float const alphaR = (-speedL * inverseDeterminant) * deltaF0
                     + inverseDeterminant * deltaF1;

  if (speedL < 0.0f) { minusH += alphaL; minusHu += alphaL * speedL; }
  else { plusH += alphaL; plusHu += alphaL * speedL; }
  // Mirror the current production CPU implementation exactly.
  if (speedR < 0.0f) {
    minusH += minusH + alphaR;
    minusHu += minusHu + alphaR * speedR;
  }
  else {
    plusH += plusH + alphaR;
    plusHu += plusHu + alphaR * speedR;
  }
  if (leftDry) { minusH = minusHu = 0.0f; }
  else if (rightDry) { plusH = plusHu = 0.0f; }
}

__global__ void copyState(float const * hOld, float const * huOld, float const * hvOld,
                          float * hNew, float * huNew, float * hvNew, std::size_t cells) {
  std::size_t const id = blockIdx.x * blockDim.x + threadIdx.x;
  if (id < cells) { hNew[id] = hOld[id]; huNew[id] = huOld[id]; hvNew[id] = hvOld[id]; }
}

__global__ void xEdges(float const * h, float const * hu, float const * b,
                       float * minusH, float * minusHu, float * plusH, float * plusHu,
                       int xCells, int yCells) {
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

__global__ void applyX(float scale, float * h, float * hu, float const * minusH,
                       float const * minusHu, float const * plusH, float const * plusHu,
                       int xCells, int yCells) {
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

__global__ void yEdges(float const * h, float const * hv, float const * b,
                       float * minusH, float * minusHv, float * plusH, float * plusHv,
                       int xCells, int yCells) {
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

__global__ void applyY(float scale, float * h, float * hv, float const * minusH,
                       float const * minusHv, float const * plusH, float const * plusHv,
                       int xCells, int yCells) {
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

__global__ void ghostColumns(float * h, float * hu, float * b, int xCells,
                             int yCells, bool reflecting) {
  int const y = blockIdx.x * blockDim.x + threadIdx.x + 1;
  if (y > yCells) return;
  int const stride = xCells + 2;
  std::size_t const leftGhost = static_cast<std::size_t>(y) * stride;
  std::size_t const rightGhost = leftGhost + xCells + 1;
  h[leftGhost] = h[leftGhost + 1]; h[rightGhost] = h[rightGhost - 1];
  b[leftGhost] = b[leftGhost + 1]; b[rightGhost] = b[rightGhost - 1];
  float const sign = reflecting ? -1.0f : 1.0f;
  hu[leftGhost] = sign * hu[leftGhost + 1];
  hu[rightGhost] = sign * hu[rightGhost - 1];
}

__global__ void ghostRows(float * h, float * hv, float * b, int xCells,
                          int yCells, bool reflecting) {
  int const x = blockIdx.x * blockDim.x + threadIdx.x + 1;
  if (x > xCells) return;
  int const stride = xCells + 2;
  std::size_t const bottomGhost = x;
  std::size_t const topGhost = static_cast<std::size_t>(yCells + 1) * stride + x;
  h[bottomGhost] = h[bottomGhost + stride]; h[topGhost] = h[topGhost - stride];
  b[bottomGhost] = b[bottomGhost + stride]; b[topGhost] = b[topGhost - stride];
  float const sign = reflecting ? -1.0f : 1.0f;
  hv[bottomGhost] = sign * hv[bottomGhost + stride];
  hv[topGhost] = sign * hv[topGhost - stride];
}

class WavePropagation2dCuda {
  int xCells, yCells, step = 0;
  bool reflecting;
  std::size_t cells, maxEdges;
  float * h[2]{}, *hu[2]{}, *hv[2]{}, *b{};
  float * edge[4]{};

  void ghosts() {
    int constexpr block = 256;
    ghostColumns<<<(yCells + block - 1) / block, block>>>(h[step], hu[step], b, xCells, yCells, reflecting);
    ghostRows<<<(xCells + block - 1) / block, block>>>(h[step], hv[step], b, xCells, yCells, reflecting);
  }

public:
  WavePropagation2dCuda(int x, int y, bool reflect): xCells(x), yCells(y), reflecting(reflect) {
    cells = static_cast<std::size_t>(x + 2) * (y + 2);
    maxEdges = std::max(static_cast<std::size_t>(x + 1) * y,
                        static_cast<std::size_t>(x) * (y + 1));
    for (int i = 0; i < 2; ++i) {
      checkCuda(cudaMalloc(&h[i], cells * sizeof(float)), "cudaMalloc h");
      checkCuda(cudaMalloc(&hu[i], cells * sizeof(float)), "cudaMalloc hu");
      checkCuda(cudaMalloc(&hv[i], cells * sizeof(float)), "cudaMalloc hv");
    }
    checkCuda(cudaMalloc(&b, cells * sizeof(float)), "cudaMalloc b");
    for (float *& pointer : edge) checkCuda(cudaMalloc(&pointer, maxEdges * sizeof(float)), "cudaMalloc edge");
  }
  ~WavePropagation2dCuda() {
    for (int i = 0; i < 2; ++i) { cudaFree(h[i]); cudaFree(hu[i]); cudaFree(hv[i]); }
    cudaFree(b); for (float * pointer : edge) cudaFree(pointer);
  }
  void upload(float const * hostH, float const * hostHu, float const * hostHv, float const * hostB) {
    std::size_t const bytes = cells * sizeof(float);
    checkCuda(cudaMemcpy(h[0], hostH, bytes, cudaMemcpyHostToDevice), "upload h");
    checkCuda(cudaMemcpy(hu[0], hostHu, bytes, cudaMemcpyHostToDevice), "upload hu");
    checkCuda(cudaMemcpy(hv[0], hostHv, bytes, cudaMemcpyHostToDevice), "upload hv");
    checkCuda(cudaMemcpy(b, hostB, bytes, cudaMemcpyHostToDevice), "upload b");
    step = 0;
  }
  void timeStep(float scale) {
    int constexpr block = 256;
    int const cellBlocks = static_cast<int>((cells + block - 1) / block);
    std::size_t const interior = static_cast<std::size_t>(xCells) * yCells;
    step = 1 - step;
    copyState<<<cellBlocks, block>>>(h[1-step], hu[1-step], hv[1-step], h[step], hu[step], hv[step], cells);
    std::size_t const xCount = static_cast<std::size_t>(xCells + 1) * yCells;
    xEdges<<<static_cast<int>((xCount + block - 1) / block), block>>>(h[1-step], hu[1-step], b,
      edge[0], edge[1], edge[2], edge[3], xCells, yCells);
    applyX<<<static_cast<int>((interior + block - 1) / block), block>>>(scale, h[step], hu[step],
      edge[0], edge[1], edge[2], edge[3], xCells, yCells);
    ghosts();

    step = 1 - step;
    copyState<<<cellBlocks, block>>>(h[1-step], hu[1-step], hv[1-step], h[step], hu[step], hv[step], cells);
    std::size_t const yCount = static_cast<std::size_t>(xCells) * (yCells + 1);
    yEdges<<<static_cast<int>((yCount + block - 1) / block), block>>>(h[1-step], hv[1-step], b,
      edge[0], edge[1], edge[2], edge[3], xCells, yCells);
    applyY<<<static_cast<int>((interior + block - 1) / block), block>>>(scale, h[step], hv[step],
      edge[0], edge[1], edge[2], edge[3], xCells, yCells);
    ghosts();
  }
  void download(float * hostH, float * hostHu, float * hostHv) {
    std::size_t const bytes = cells * sizeof(float);
    checkCuda(cudaMemcpy(hostH, h[step], bytes, cudaMemcpyDeviceToHost), "download h");
    checkCuda(cudaMemcpy(hostHu, hu[step], bytes, cudaMemcpyDeviceToHost), "download hu");
    checkCuda(cudaMemcpy(hostHv, hv[step], bytes, cudaMemcpyDeviceToHost), "download hv");
  }
};

void initialize(tsunami_lab::patches::WavePropagation2d & patch, int xCells, int yCells) {
  for (int y = 0; y < yCells; ++y) for (int x = 0; x < xCells; ++x) {
    float const px = static_cast<float>(x) / xCells;
    float const py = static_cast<float>(y) / yCells;
    float const height = 5.0f + 0.2f * sinf(6.2831853f * px) * cosf(6.2831853f * py);
    patch.setHeight(x, y, height);
    patch.setMomentumX(x, y, 0.1f * height * sinf(3.1415926f * py));
    patch.setMomentumY(x, y, 0.1f * height * cosf(3.1415926f * px));
    patch.setBathymetry(x, y, -10.0f + 0.1f * sinf(3.1415926f * px));
  }
  patch.setGhostCollumn();
}
} // namespace

int main(int argc, char ** argv) {
  int const n = argc > 1 ? std::atoi(argv[1]) : 512;
  int const iterations = argc > 2 ? std::atoi(argv[2]) : 20;
  if (n <= 0 || iterations <= 0) return EXIT_FAILURE;
  float constexpr scale = 0.0001f;
  bool constexpr reflecting = false;
  std::size_t const cells = static_cast<std::size_t>(n + 2) * (n + 2);

  omp_set_num_threads(1);
  tsunami_lab::patches::WavePropagation2d serial(n, n, false, reflecting);
  initialize(serial, n, n);
  std::vector<float> initialH(serial.getHeight(), serial.getHeight() + cells);
  std::vector<float> initialHu(serial.getMomentumX(), serial.getMomentumX() + cells);
  std::vector<float> initialHv(serial.getMomentumY(), serial.getMomentumY() + cells);
  std::vector<float> initialB(serial.getBathymetry(), serial.getBathymetry() + cells);

  auto startCpu = std::chrono::steady_clock::now();
  for (int i = 0; i < iterations; ++i) serial.timeStep(scale);
  auto stopCpu = std::chrono::steady_clock::now();
  double const serialMs = std::chrono::duration<double, std::milli>(stopCpu - startCpu).count() / iterations;

  omp_set_num_threads(16);
  tsunami_lab::patches::WavePropagation2d parallel(n, n, false, reflecting);
  initialize(parallel, n, n);
  auto startOmp = std::chrono::steady_clock::now();
  for (int i = 0; i < iterations; ++i) parallel.timeStep(scale);
  auto stopOmp = std::chrono::steady_clock::now();
  double const openmpMs = std::chrono::duration<double, std::milli>(stopOmp - startOmp).count() / iterations;

  WavePropagation2dCuda cudaPatch(n, n, reflecting);
  cudaPatch.upload(initialH.data(), initialHu.data(), initialHv.data(), initialB.data());
  cudaEvent_t gpuStart, gpuStop;
  checkCuda(cudaEventCreate(&gpuStart), "event create"); checkCuda(cudaEventCreate(&gpuStop), "event create");
  checkCuda(cudaEventRecord(gpuStart), "event record");
  for (int i = 0; i < iterations; ++i) cudaPatch.timeStep(scale);
  checkCuda(cudaEventRecord(gpuStop), "event record"); checkCuda(cudaEventSynchronize(gpuStop), "event sync");
  float totalGpuMs = 0.0f; checkCuda(cudaEventElapsedTime(&totalGpuMs, gpuStart, gpuStop), "event elapsed");
  float const cudaMs = totalGpuMs / iterations;

  std::vector<float> gpuH(cells), gpuHu(cells), gpuHv(cells);
  cudaPatch.download(gpuH.data(), gpuHu.data(), gpuHv.data());
  std::size_t mismatches = 0;
  float maxError = 0.0f;
  auto compare = [&](float const * cpu, std::vector<float> const & gpu) {
    for (std::size_t i = 0; i < cells; ++i) {
      float const error = std::fabs(cpu[i] - gpu[i]);
      maxError = std::max(maxError, error);
      if (!std::isfinite(gpu[i]) || error > 2.0e-4f + 2.0e-4f * std::fabs(cpu[i])) ++mismatches;
    }
  };
  compare(serial.getHeight(), gpuH); compare(serial.getMomentumX(), gpuHu); compare(serial.getMomentumY(), gpuHv);

  std::cout << std::fixed << std::setprecision(3)
            << "WavePropagation2d F-Wave benchmark (" << n << "x" << n << ", " << iterations << " steps)\n"
            << "CPU serial:  " << serialMs << " ms/step\n"
            << "CPU OpenMP:  " << openmpMs << " ms/step\n"
            << "CUDA resident: " << cudaMs << " ms/step\n"
            << "OpenMP speedup: " << serialMs / openmpMs << "x\n"
            << "CUDA speedup:   " << serialMs / cudaMs << "x vs. serial, "
            << openmpMs / cudaMs << "x vs. OpenMP\n"
            << "Max error: " << std::scientific << maxError << ", mismatches: " << mismatches
            << " / " << 3 * cells << '\n';
  cudaEventDestroy(gpuStart); cudaEventDestroy(gpuStop);
  return mismatches == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

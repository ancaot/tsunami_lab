/**
 * Standalone CUDA prototype for batched f-wave net updates.
 * One CUDA thread computes one edge; results are checked against the CPU solver.
 */
#include <cuda_runtime.h>
#include "../src/solvers/fwave.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr float g = 9.80665f;
constexpr float gSqrt = 3.131557121f;

#define CUDA_CHECK(call)                                                        \
  do {                                                                          \
    cudaError_t const error = (call);                                            \
    if (error != cudaSuccess) {                                                  \
      std::cerr << "CUDA error at " << __FILE__ << ':' << __LINE__ << ": "     \
                << cudaGetErrorString(error) << '\n';                            \
      return EXIT_FAILURE;                                                       \
    }                                                                            \
  } while (false)

__device__ void fwaveNetUpdates(float hL, float hR, float huL, float huR,
                                float bL, float bR, float & minusH,
                                float & minusHu, float & plusH, float & plusHu) {
  minusH = 0.0f;
  minusHu = 0.0f;
  plusH = 0.0f;
  plusHu = 0.0f;
  bool const leftWasDry = hL <= 0.0f;
  bool const rightWasDry = hR <= 0.0f;

  if (hL <= 0.0f) {
    if (hR <= 0.0f) return;
    hL = hR;
    huL = -huR;
    bL = bR;
  }
  else if (hR <= 0.0f) {
    hR = hL;
    huR = -huL;
    bR = bL;
  }

  float const sqrtHL = sqrtf(hL);
  float const sqrtHR = sqrtf(hR);
  float const hRoe = 0.5f * (hL + hR);
  float uRoe = sqrtHL * (huL / hL) + sqrtHR * (huR / hR);
  uRoe /= sqrtHL + sqrtHR;
  float const sqrtGhRoe = gSqrt * sqrtf(hRoe);
  float const speedL = uRoe - sqrtGhRoe;
  float const speedR = uRoe + sqrtGhRoe;

  float const huSquaredL = huL * huL / hL;
  float const gHeightSquaredL = g * hL * hL;
  float const totalL = huSquaredL + 0.5f * gHeightSquaredL;
  float const huSquaredR = huR * huR / hR;
  float const gHeightSquaredR = g * hR * hR;
  float const totalR = huSquaredR + 0.5f * gHeightSquaredR;
  float const deltaF0 = huR - huL;
  float deltaF1 = totalR - totalL;
  float const bathymetry = (-g) * (bR - bL) * ((hL + hR) / 2.0f);
  deltaF1 -= bathymetry;

  float const invDet = 1.0f / (speedR - speedL);
  float const inverse0 = speedR * invDet;
  float const inverse1 = -invDet;
  float const inverse2 = -speedL * invDet;
  float const inverse3 = invDet;
  float const alphaL = inverse0 * deltaF0 + inverse1 * deltaF1;
  float const alphaR = inverse2 * deltaF0 + inverse3 * deltaF1;

  if (speedL < 0.0f) {
    minusH += alphaL;
    minusHu += alphaL * speedL;
  }
  else {
    plusH += alphaL;
    plusHu += alphaL * speedL;
  }

  // Deliberately mirrors the current CPU fwave::decompose behavior.
  if (speedR < 0.0f) {
    minusH += minusH + alphaR;
    minusHu += minusHu + alphaR * speedR;
  }
  else {
    plusH += plusH + alphaR;
    plusHu += plusHu + alphaR * speedR;
  }

  // The CPU implementation redirects the corresponding output pointer to a
  // temporary buffer at a reflecting dry boundary.
  if (leftWasDry) {
    minusH = 0.0f;
    minusHu = 0.0f;
  }
  else if (rightWasDry) {
    plusH = 0.0f;
    plusHu = 0.0f;
  }
}

__global__ void fwaveNetUpdatesKernel(float const * hL, float const * hR,
                                      float const * huL, float const * huR,
                                      float const * bL, float const * bR,
                                      float * minusH, float * minusHu,
                                      float * plusH, float * plusHu,
                                      std::size_t count) {
  std::size_t const edge = blockIdx.x * blockDim.x + threadIdx.x;
  if (edge < count) {
    fwaveNetUpdates(hL[edge], hR[edge], huL[edge], huR[edge], bL[edge], bR[edge],
                    minusH[edge], minusHu[edge], plusH[edge], plusHu[edge]);
  }
}

float elapsedMs(cudaEvent_t start, cudaEvent_t stop) {
  float milliseconds = 0.0f;
  cudaEventElapsedTime(&milliseconds, start, stop);
  return milliseconds;
}
} // namespace

int main(int argc, char ** argv) {
  std::size_t count = 1'000'000;
  int iterations = 100;
  try {
    if (argc > 1) count = std::stoull(argv[1]);
    if (argc > 2) iterations = std::stoi(argv[2]);
  }
  catch (std::exception const & exception) {
    std::cerr << "Invalid argument: " << exception.what() << '\n';
    return EXIT_FAILURE;
  }
  if (count == 0 || iterations <= 0) {
    std::cerr << "Usage: fwave_benchmark [edge-count > 0] [iterations > 0]\n";
    return EXIT_FAILURE;
  }

  int deviceCount = 0;
  CUDA_CHECK(cudaGetDeviceCount(&deviceCount));
  if (deviceCount == 0) {
    std::cerr << "No CUDA-capable GPU found.\n";
    return EXIT_FAILURE;
  }
  cudaDeviceProp properties{};
  CUDA_CHECK(cudaGetDeviceProperties(&properties, 0));

  std::vector<float> hL(count), hR(count), huL(count), huR(count), bL(count), bR(count);
  std::vector<float> cpuMinusH(count), cpuMinusHu(count), cpuPlusH(count), cpuPlusHu(count);
  std::vector<float> gpuMinusH(count), gpuMinusHu(count), gpuPlusH(count), gpuPlusHu(count);

  for (std::size_t edge = 0; edge < count; ++edge) {
    float const phase = static_cast<float>(edge % 10007) * 0.001f;
    hL[edge] = 0.25f + 9.75f * (0.5f + 0.5f * std::sin(phase));
    hR[edge] = 0.25f + 9.75f * (0.5f + 0.5f * std::cos(phase * 0.91f));
    huL[edge] = hL[edge] * (1.5f * std::sin(phase * 0.37f));
    huR[edge] = hR[edge] * (1.5f * std::cos(phase * 0.43f));
    bL[edge] = -20.0f + 2.0f * std::sin(phase * 0.17f);
    bR[edge] = -20.0f + 2.0f * std::cos(phase * 0.13f);
    if (edge % 997 == 0) hL[edge] = 0.0f;
    if (edge % 1597 == 0) hR[edge] = 0.0f;
  }

  auto const cpuStart = std::chrono::steady_clock::now();
  for (std::size_t edge = 0; edge < count; ++edge) {
    float minus[2];
    float plus[2];
    tsunami_lab::solvers::fwave::netUpdates(hL[edge], hR[edge], huL[edge], huR[edge],
                                             bL[edge], bR[edge], minus, plus);
    cpuMinusH[edge] = minus[0];
    cpuMinusHu[edge] = minus[1];
    cpuPlusH[edge] = plus[0];
    cpuPlusHu[edge] = plus[1];
  }
  auto const cpuStop = std::chrono::steady_clock::now();
  double const cpuMs = std::chrono::duration<double, std::milli>(cpuStop - cpuStart).count();

  float * device[10]{};
  std::size_t const bytes = count * sizeof(float);
  auto const allocationStart = std::chrono::steady_clock::now();
  for (float *& pointer : device) CUDA_CHECK(cudaMalloc(&pointer, bytes));
  auto const allocationStop = std::chrono::steady_clock::now();
  double const allocationMs = std::chrono::duration<double, std::milli>(allocationStop - allocationStart).count();

  cudaEvent_t start = nullptr;
  cudaEvent_t stop = nullptr;
  CUDA_CHECK(cudaEventCreate(&start));
  CUDA_CHECK(cudaEventCreate(&stop));

  CUDA_CHECK(cudaEventRecord(start));
  CUDA_CHECK(cudaMemcpy(device[0], hL.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(device[1], hR.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(device[2], huL.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(device[3], huR.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(device[4], bL.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(device[5], bR.data(), bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));
  float const hostToDeviceMs = elapsedMs(start, stop);

  int constexpr blockSize = 256;
  int const gridSize = static_cast<int>((count + blockSize - 1) / blockSize);
  fwaveNetUpdatesKernel<<<gridSize, blockSize>>>(device[0], device[1], device[2], device[3], device[4],
    device[5], device[6], device[7], device[8], device[9], count);
  CUDA_CHECK(cudaGetLastError());
  CUDA_CHECK(cudaDeviceSynchronize());

  CUDA_CHECK(cudaEventRecord(start));
  for (int iteration = 0; iteration < iterations; ++iteration) {
    fwaveNetUpdatesKernel<<<gridSize, blockSize>>>(device[0], device[1], device[2], device[3], device[4],
      device[5], device[6], device[7], device[8], device[9], count);
  }
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));
  CUDA_CHECK(cudaGetLastError());
  float const kernelMs = elapsedMs(start, stop) / static_cast<float>(iterations);

  CUDA_CHECK(cudaEventRecord(start));
  CUDA_CHECK(cudaMemcpy(gpuMinusH.data(), device[6], bytes, cudaMemcpyDeviceToHost));
  CUDA_CHECK(cudaMemcpy(gpuMinusHu.data(), device[7], bytes, cudaMemcpyDeviceToHost));
  CUDA_CHECK(cudaMemcpy(gpuPlusH.data(), device[8], bytes, cudaMemcpyDeviceToHost));
  CUDA_CHECK(cudaMemcpy(gpuPlusHu.data(), device[9], bytes, cudaMemcpyDeviceToHost));
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));
  float const deviceToHostMs = elapsedMs(start, stop);

  std::size_t mismatches = 0;
  float maxAbsoluteError = 0.0f;
  float maxRelativeError = 0.0f;
  auto compare = [&](char const * component, std::vector<float> const & cpu,
                     std::vector<float> const & gpu) {
    for (std::size_t edge = 0; edge < count; ++edge) {
      float const absolute = std::fabs(cpu[edge] - gpu[edge]);
      float const relative = absolute / std::max(std::fabs(cpu[edge]), 1.0e-6f);
      maxAbsoluteError = std::max(maxAbsoluteError, absolute);
      maxRelativeError = std::max(maxRelativeError, relative);
      if (!std::isfinite(gpu[edge]) || absolute > 1.0e-4f + 1.0e-4f * std::fabs(cpu[edge])) {
        if (mismatches < 5) {
          std::cerr << "Mismatch " << component << " at edge " << edge
                    << ": CPU=" << cpu[edge] << ", CUDA=" << gpu[edge]
                    << ", hL=" << hL[edge] << ", hR=" << hR[edge] << '\n';
        }
        ++mismatches;
      }
    }
  };
  compare("minus height", cpuMinusH, gpuMinusH);
  compare("minus momentum", cpuMinusHu, gpuMinusHu);
  compare("plus height", cpuPlusH, gpuPlusH);
  compare("plus momentum", cpuPlusHu, gpuPlusHu);

  for (float * pointer : device) CUDA_CHECK(cudaFree(pointer));
  CUDA_CHECK(cudaEventDestroy(start));
  CUDA_CHECK(cudaEventDestroy(stop));

  float const endToEndMs = hostToDeviceMs + kernelMs + deviceToHostMs;
  std::cout << std::fixed << std::setprecision(3)
            << "GPU:                 " << properties.name << '\n'
            << "Edges:               " << count << '\n'
            << "GPU allocation:      " << allocationMs << " ms (" << (10 * bytes / 1.0e6) << " MB)\n"
            << "CPU net updates:     " << cpuMs << " ms\n"
            << "Host -> device:      " << hostToDeviceMs << " ms\n"
            << "CUDA kernel:         " << kernelMs << " ms (average of " << iterations << ")\n"
            << "Device -> host:      " << deviceToHostMs << " ms\n"
            << "CUDA end-to-end:     " << endToEndMs << " ms\n"
            << "Kernel speedup:      " << (cpuMs / kernelMs) << "x\n"
            << "End-to-end speedup:  " << (cpuMs / endToEndMs) << "x\n"
            << std::scientific
            << "Maximum abs. error:  " << maxAbsoluteError << '\n'
            << "Maximum rel. error:  " << maxRelativeError << '\n'
            << "Mismatches:          " << mismatches << " / " << (4 * count) << '\n';

  if (mismatches != 0) {
    std::cerr << "CPU/CUDA comparison failed.\n";
    return EXIT_FAILURE;
  }
  std::cout << "CPU/CUDA comparison passed.\n";
  return EXIT_SUCCESS;
}

/**
 * Minimal CUDA smoke test for the tsunami_lab project.
 *
 * This file intentionally stays independent from the SCons build. It verifies
 * the local CUDA compiler, runtime, GPU execution, and host/device copies
 * before we start porting tsunami-specific kernels.
 */
#include <cuda_runtime.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#define CUDA_CHECK(call)                                                        \
  do {                                                                         \
    cudaError_t err = (call);                                                   \
    if (err != cudaSuccess) {                                                   \
      std::cerr << "CUDA error at " << __FILE__ << ":" << __LINE__ << ": "    \
                << cudaGetErrorString(err) << std::endl;                       \
      return EXIT_FAILURE;                                                      \
    }                                                                          \
  } while (0)

__global__ void addScaledKernel(float const * a,
                                float const * b,
                                float       * out,
                                int           n,
                                float         scale) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < n) {
    out[idx] = a[idx] + scale * b[idx];
  }
}

int main() {
  int deviceCount = 0;
  CUDA_CHECK(cudaGetDeviceCount(&deviceCount));

  if (deviceCount == 0) {
    std::cerr << "No CUDA-capable GPU found." << std::endl;
    return EXIT_FAILURE;
  }

  cudaDeviceProp props{};
  CUDA_CHECK(cudaGetDeviceProperties(&props, 0));
  std::cout << "Using GPU: " << props.name << std::endl;

  constexpr int n = 1024;
  constexpr float scale = 0.5f;
  std::vector<float> a(n);
  std::vector<float> b(n);
  std::vector<float> out(n, 0.0f);

  for (int i = 0; i < n; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(2 * i);
  }

  float * d_a = nullptr;
  float * d_b = nullptr;
  float * d_out = nullptr;

  CUDA_CHECK(cudaMalloc(&d_a, n * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_b, n * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_out, n * sizeof(float)));

  CUDA_CHECK(cudaMemcpy(d_a, a.data(), n * sizeof(float), cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_b, b.data(), n * sizeof(float), cudaMemcpyHostToDevice));

  constexpr int blockSize = 256;
  int gridSize = (n + blockSize - 1) / blockSize;
  addScaledKernel<<<gridSize, blockSize>>>(d_a, d_b, d_out, n, scale);
  CUDA_CHECK(cudaGetLastError());
  CUDA_CHECK(cudaDeviceSynchronize());

  CUDA_CHECK(cudaMemcpy(out.data(), d_out, n * sizeof(float), cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_a));
  CUDA_CHECK(cudaFree(d_b));
  CUDA_CHECK(cudaFree(d_out));

  for (int i = 0; i < n; ++i) {
    float expected = a[i] + scale * b[i];
    if (std::fabs(out[i] - expected) > 1e-6f) {
      std::cerr << "Mismatch at index " << i << ": got " << out[i]
                << ", expected " << expected << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cout << "CUDA smoke test passed for " << n << " values." << std::endl;
  return EXIT_SUCCESS;
}

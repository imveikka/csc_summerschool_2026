// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

/*
 * This code in its current form uses the default stream
 * Task:
 * - Place kernel_{a,b,c} in separate streams and execute them asynchronously
 * - Validate that kernels are executing concurrently with `run_tue ... rocprof --hip-trace ./<your->
 *   - Open chromium url chrome://tracing or https://ui.perfetto.dev, open file "results.json"
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../error_checking.hpp"

// GPU kernel definition
__global__ void kernel_a(float *a, int n)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  // Evaluate the trigonometric identity
  // sin^2(x) + cos^2(x) = 1
  // Very light kernel, one sin/cos evaluation per element
  if (tid < n) {
    float x = 0.001f * float(tid % 1000);
    float s = sinf(x);
    float c = cosf(x);

    a[tid] = s * s + c * c;
  }
}


__global__ void kernel_b(float *b, int n)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  // Heavy kernel: repeatedly updates x using sine, cosine, and arctangent
  // Converges to 1.313534
  if (tid < n) {
    float x = 0.001f * float(tid % 1000) + 1.0f;

    for (int i = 0; i < 200; ++i) {
      x = sinf(x) + cosf(x) + 0.1f * atanf(x);
    }

    b[tid] = x;
  }
}

__global__ void kernel_c(float *c, int n)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  if (tid < n) {
    float x = 0.001f * float(tid % 1000);

    // Fixed-point iteration for cos(x) = x.
    // Converges to ~0.739085
    // Medium
    for (int i = 0; i < 50; ++i) {
      x = cosf(x);
    }

    c[tid] = x;
  }
}

int main() {
  constexpr size_t N = 1<<24; // ~64 MiB array

  constexpr int blocksize = 256;
  constexpr int gridsize =(N-1+blocksize)/blocksize;
  constexpr size_t N_bytes = N*sizeof(float);

  // Host & device pointers
  float *a; float *d_a;
  float *b; float *d_b;
  float *c; float *d_c;

  // Events
  hipEvent_t start_a, start_b, start_c, end_a, end_b, end_c;
  HIP_ERRCHK(hipEventCreate(&start_a));
  HIP_ERRCHK(hipEventCreate(&start_b));
  HIP_ERRCHK(hipEventCreate(&start_c));
  HIP_ERRCHK(hipEventCreate(&end_a));
  HIP_ERRCHK(hipEventCreate(&end_b));
  HIP_ERRCHK(hipEventCreate(&end_c));

  // Host allocations
  HIP_ERRCHK(hipHostMalloc((void**)&a, N_bytes));
  HIP_ERRCHK(hipHostMalloc((void**)&b, N_bytes));
  HIP_ERRCHK(hipHostMalloc((void**)&c, N_bytes));

  // Device allocations
  HIP_ERRCHK(hipMalloc((void**)&d_a, N_bytes));
  HIP_ERRCHK(hipMalloc((void**)&d_b, N_bytes));
  HIP_ERRCHK(hipMalloc((void**)&d_c, N_bytes));

  // warmup
  kernel_c<<<gridsize, blocksize>>>(d_a, N);
  HIP_ERRCHK(hipMemcpy(a, d_a, N_bytes/100, hipMemcpyDefault));
  HIP_ERRCHK(hipDeviceSynchronize());

  // Execute kernels in sequence
  hipStream_t streams[3];
  for (int i=0; i<3; i++) {
    HIP_ERRCHK(hipStreamCreate(&streams[i]));
  }

  HIP_ERRCHK(hipMemcpyAsync(d_a, a, N_bytes, hipMemcpyHostToDevice, streams[0]));
  HIP_ERRCHK(hipEventRecord(start_a, streams[0]));
  kernel_a<<<gridsize, blocksize, 0, streams[0]>>>(d_a, N);
  HIP_ERRCHK(hipEventRecord(end_a, streams[0]));
  HIP_ERRCHK(hipMemcpyAsync(a, d_a, N_bytes, hipMemcpyDeviceToHost, streams[0]));

  HIP_ERRCHK(hipMemcpyAsync(d_b, b, N_bytes, hipMemcpyHostToDevice, streams[1]));
  HIP_ERRCHK(hipEventRecord(start_b, streams[1]));
  kernel_b<<<gridsize, blocksize, 0, streams[1]>>>(d_b, N);
  HIP_ERRCHK(hipEventRecord(end_b, streams[1]));
  HIP_ERRCHK(hipMemcpyAsync(b, d_b, N_bytes, hipMemcpyDeviceToHost, streams[1]));

  HIP_ERRCHK(hipMemcpyAsync(d_c, c, N_bytes, hipMemcpyHostToDevice, streams[2]));
  HIP_ERRCHK(hipEventRecord(start_c, streams[2]));
  kernel_c<<<gridsize, blocksize, 0, streams[2]>>>(d_c, N);
  HIP_ERRCHK(hipEventRecord(end_c, streams[2]));
  HIP_ERRCHK(hipGetLastError());
  HIP_ERRCHK(hipMemcpyAsync(c, d_c, N_bytes, hipMemcpyDeviceToHost, streams[2]));

  for (int i=0; i<3; i++) {
    HIP_ERRCHK(hipStreamSynchronize(streams[i]));
    HIP_ERRCHK(hipStreamDestroy(streams[i]));
  }

  for (int i = 0; i < 10; ++i) printf("%f ", a[i]);
  printf("\n");
  float ms_a;
  HIP_ERRCHK(hipEventElapsedTime(&ms_a, start_a, end_a));
  printf("Time elapsed: %.3f ms.\n", ms_a);

  for (int i = 0; i < 10; ++i) printf("%f ", b[i]);
  printf("\n");
  float ms_b;
  HIP_ERRCHK(hipEventElapsedTime(&ms_b, start_b, end_b));
  printf("Time elapsed: %.3f ms.\n", ms_b);

  for (int i = 0; i < 10; ++i) printf("%f ", c[i]);
  printf("\n");
  float ms_c;
  HIP_ERRCHK(hipEventElapsedTime(&ms_c, start_c, end_c));
  printf("Time elapsed: %.3f ms.\n", ms_c);

  // Free device, events and host memory allocations

  HIP_ERRCHK(hipEventDestroy(start_a));
  HIP_ERRCHK(hipEventDestroy(start_b));
  HIP_ERRCHK(hipEventDestroy(start_c));
  HIP_ERRCHK(hipEventDestroy(end_a));
  HIP_ERRCHK(hipEventDestroy(end_b));
  HIP_ERRCHK(hipEventDestroy(end_c));

  HIP_ERRCHK(hipFree(d_a));
  HIP_ERRCHK(hipFree(d_b));
  HIP_ERRCHK(hipFree(d_c));

  HIP_ERRCHK(hipHostFree(a));
  HIP_ERRCHK(hipHostFree(b));
  HIP_ERRCHK(hipHostFree(c));
}

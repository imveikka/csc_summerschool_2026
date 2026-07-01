// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <hip/hip_runtime.h>
#include <math.h>
#include <stdio.h>
#include <vector>

#include "../error_checking.hpp"

// Copy all elements using threads in a 2D grid
__global__ void copy2d(double *dst, double *src, const int rows, const int cols) {
    // TODO: compute row and col using
    // - threadIdx.x, threadIdx.y
    // - blockIdx.x, blockIdx.y
    // - blockDim.x, blockDim.y

    const int tid_x = threadIdx.x + blockIdx.x * blockDim.x;
    const int stride_x = blockDim.x * gridDim.x;
    const int tid_y = threadIdx.y + blockIdx.y * blockDim.y;
    const int stride_y = blockDim.y * gridDim.y;

    // TODO: Make sure there's no out-of-bounds access
    // row must be < number of rows
    // col must be < number of columns

    for (int row = tid_x; row < rows; row += stride_x) {
        for (int col = tid_y; col < cols; col += stride_y) {
            const size_t index = row * cols + col;
            dst[index] = src[index];
        }
    }

    // We're computing 1D index from a 2D index and copying from src to dst
}

int main() {
    static constexpr size_t num_cols = 600;
    static constexpr size_t num_rows = 400;
    static constexpr size_t num_values = num_cols * num_rows;
    static constexpr size_t num_bytes = sizeof(double) * num_values;
    std::vector<double> x(num_values);
    std::vector<double> y(num_values, 0.0);

    // Initialise data
    for (size_t i = 0; i < num_values; i++) {
        x[i] = static_cast<double>(i) / 1000.0;
    }

    // TODO: Allocate + copy initial values to GPU
    double *x_g = nullptr;
    HIP_ERRCHK(hipMalloc(&x_g, num_bytes));
    double *y_g = nullptr;
    HIP_ERRCHK(hipMalloc(&y_g, num_bytes));
    HIP_ERRCHK(hipMemcpy(x_g, static_cast<void *>(x.data()), num_bytes, hipMemcpyDefault));

    // TODO: Define grid dimensions
    // Use dim3 structure for threads and blocks
    dim3 threads(128, 1, 1);
    dim3 blocks(128, 1, 1);

    // TODO: launch the device kernel
    LAUNCH_KERNEL(copy2d, blocks, threads, 0, 0, y_g, x_g, (int) num_rows, (int) num_cols);

    // TODO: Copy results back to the CPU vector y
    HIP_ERRCHK(hipMemcpy(static_cast<void *>(y.data()), y_g, num_bytes, hipMemcpyDefault));

    // TODO: Free device memory
    HIP_ERRCHK(hipFree(x_g));
    HIP_ERRCHK(hipFree(y_g));

    // Check result of computation on the GPU
    double error = 0.0;
    for (size_t i = 0; i < num_values; i++) {
        error += abs(x[i] - y[i]);
    }

    printf("total error: %f\n", error);
    printf("  reference: %f at (42,42)\n", x[42 * num_rows + 42]);
    printf("     result: %f at (42,42)\n", y[42 * num_rows + 42]);

    return 0;
}

/*
 * Bonus:
 * Once you get the 2D copy kernel working,
 * try to implement it by reusing threads in the kernel.
 * In other words, write a double for loop inside the kernel
 * such that each thread may process multiple elements.
 * You can write is as a new kernel (e.g. copy2dForLoop)
 * so you don't need to change the main function except for
 * the kernel launch.
 *
 * If you need some help, check out the solution OR check
 * the 1D for-loop exercise under fundamentals.
 *
 * Extra bonus:
 * Try to implement a 3D copy with and without a triple loop.
 * */

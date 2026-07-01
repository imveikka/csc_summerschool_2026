// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

/*
 * This header includes error checking functions and macros
 * that can be used in the exercises here.
 * Just #include this file.
 * */

#include <hip/hip_runtime.h>
#include <cstdio>

#define LAUNCH_KERNEL(kernel, ...)                                             \
    launch_kernel(#kernel, __FILE__, __LINE__, kernel, __VA_ARGS__)
template <typename... Args>
void launch_kernel(const char *kernel_name, const char *file, int32_t line,
                   void (*kernel)(Args...), dim3 blocks, dim3 threads,
                   size_t num_bytes_shared_mem, hipStream_t stream,
                   Args... args) {
    int32_t device = 0;
    [[maybe_unused]] auto result = hipGetDevice(&device);

    // Helper lambda for querying device attributes
    auto get_device_attribute = [&device](hipDeviceAttribute_t attribute) {
        int32_t value = 0;
        [[maybe_unused]] const auto result =
            hipDeviceGetAttribute(&value, attribute, device);
        return value;
    };

    // Let's query from the API what's the maximum amount of shared memory per
    // block.
    const int32_t max_shared_memory_per_block = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxSharedMemoryPerBlock);

    // Next, let's make sure the number of bytes the user gives is not more than
    // the maximum. If it is, we'll print a helpful message and exit the program
    // immediately.
    if (num_bytes_shared_mem > max_shared_memory_per_block) {
        std::fprintf(stderr,
                     "Shared memory request too large: %ld > %d, for kernel "
                     "\"%s\" in %s on line %d\n",
                     num_bytes_shared_mem, max_shared_memory_per_block,
                     kernel_name, file, line);
        exit(EXIT_FAILURE);
    }

    // Check the requested number of threads per block is within the
    // correct limits. It must be greater than zero and less than or equal to
    // the maximum.
    const int max_threads_x = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxBlockDimX);
    const int max_threads_y = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxBlockDimY);
    const int max_threads_z = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxBlockDimZ);
    if (threads.x <= 0 || max_threads_x < threads.x) {
        std::fprintf(stderr,
                     "threads.x=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     threads.x, max_threads_x, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }
    if (threads.y <= 0 || max_threads_y < threads.y) {
        std::fprintf(stderr,
                     "threads.y=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     threads.y, max_threads_y, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }
    if (threads.z <= 0 || max_threads_z < threads.z) {
        std::fprintf(stderr,
                     "threads.z=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     threads.z, max_threads_z, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }


    const int max_blocks_x = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxGridDimX);
    const int max_blocks_y = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxGridDimY);
    const int max_blocks_z = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxGridDimZ);
    if (blocks.x <= 0 || max_blocks_x < blocks.x) {
        std::fprintf(stderr,
                     "blocks.x=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     blocks.x, max_blocks_x, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }
    if (blocks.y <= 0 || max_blocks_y < blocks.y) {
        std::fprintf(stderr,
                     "blocks.y=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     blocks.y, max_blocks_y, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }
    if (blocks.x <= 0 || max_blocks_z < blocks.z) {
        std::fprintf(stderr,
                     "blocks.z=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     blocks.z, max_blocks_z, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }


    const int max_threads = get_device_attribute(
        hipDeviceAttribute_t::hipDeviceAttributeMaxThreadsPerBlock);
    const long total_threads = threads.x * threads.y * threads.z;
    if (total_threads <= 0 || max_threads < total_threads) {
        std::fprintf(stderr,
                     "total_threads=%ld not in (0, %d] for kernel "
                     "\"%s\" in %s on line %d\n",
                     total_threads, max_threads, kernel_name, file, line);
        exit(EXIT_FAILURE);
    }

    // Reset the error variable to success.
    result = hipGetLastError();

    kernel<<<blocks, threads, num_bytes_shared_mem, stream>>>(args...);

    result = hipGetLastError();
    if (result != hipSuccess) {
        printf("Error with kernel \"%s\" in %s at line %d\n%s: %s\n",
               kernel_name, file, line, hipGetErrorName(result),
               hipGetErrorString(result));
        exit(EXIT_FAILURE);
    }
}

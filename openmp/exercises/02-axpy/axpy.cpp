// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <string>
#include <vector>
#include "helper_functions.hpp"
#include <omp.h>

int main(int argc, char* argv[]) {
    // Array size
    int n = 102400;
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }
    printf("Array size n = %d\n", n);

    double alpha;
    std::vector<double> x(n), y(n);

    // Initialization
    alpha = 3.0;
    for (int i = 0; i < n; i++) {
        double frac = 1.0 / ((double) (n - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x);
    print_array("y", y);

    // Calculate axpy
    double t0 = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < n; i++) y[i] += alpha * x[i];
    double t1 = omp_get_wtime();

    // Print output values
    printf("Output:\n");
    print_array("y", y);

    printf("Time elapsed: %.3f s.\n", t1 - t0);

    return 0;
}

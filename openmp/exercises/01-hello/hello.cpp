// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>

#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
    printf("Hello world!\n");

    #pragma omp parallel
    {
#ifdef _OPENMP
        int numt = omp_get_num_threads();
        int tid = omp_get_thread_num();
        printf("Hello from world from thread %d/%d!\n", tid, numt);
#endif
    }

    return 0;
}

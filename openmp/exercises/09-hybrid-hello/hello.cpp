// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <omp.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank = 0;
    int provided, required=MPI_THREAD_FUNNELED;
    MPI_Init_thread(&argc, &argv,
                    required, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello from thread %d in process %d!\n", tid, rank);
    }
    if (rank == 0) {
        printf("Provided thread support: %d.\n", provided);
        printf("MPI_THREAD_SIGNLE: %d.\n", MPI_THREAD_SINGLE);
        printf("MPI_THREAD_FUNNELED: %d.\n", MPI_THREAD_FUNNELED);
        printf("MPI_THREAD_SERIALIZED: %d.\n", MPI_THREAD_SERIALIZED);
        printf("MPI_THREAD_MULTIPLE: %d.\n", MPI_THREAD_MULTIPLE);
    }

    MPI_Finalize();

    return 0;
}

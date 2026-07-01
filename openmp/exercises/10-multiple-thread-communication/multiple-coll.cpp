// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
    int rank, ntasks;

    int provided, required=MPI_THREAD_MULTIPLE;
    MPI_Init_thread(&argc, &argv,
                    required, &provided);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    std::vector<MPI_Comm> thread_comms(omp_get_max_threads());
    for (MPI_Comm& comm: thread_comms) MPI_Comm_dup(MPI_COMM_WORLD, &comm);

#pragma omp parallel
{
    int tid = omp_get_thread_num();
    int msg;

    if (rank == 0) {
        msg = tid;
    }

    MPI_Bcast(&msg, 1, MPI_INT, 0, thread_comms[tid]);

    if (rank > 0) {
        printf("Rank %d thread %d received %d\n", rank, tid, msg);
    }
}

    for (MPI_Comm& comm: thread_comms) MPI_Comm_free(&comm);

    MPI_Finalize();
    return 0;
}

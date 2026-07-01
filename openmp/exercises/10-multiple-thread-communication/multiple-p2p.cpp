// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <mpi.h>
#include <omp.h>


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
    int msg = -1;
    int tag = 0;

    if (rank == 0) {
        msg = tid;
        for (int i = 1; i < ntasks; i++) {
            MPI_Send(&msg, 1, MPI_INT, i, tag, thread_comms[tid]);
        }
    } else {
        MPI_Recv(&msg, 1, MPI_INT, 0, tag, thread_comms[tid], MPI_STATUS_IGNORE);
        printf("Rank %d thread %d received %d\n", rank, tid, msg);
    }
}
    for (MPI_Comm& comm: thread_comms) MPI_Comm_free(&comm);

    MPI_Finalize();
    return 0;
}

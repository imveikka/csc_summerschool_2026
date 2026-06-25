// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <vector>
#include <mpi.h>

#define NTASKS 4

void init_buffers(std::vector<int> &sendbuffer, std::vector<int> &recvbuffer);
void print_buffers(std::vector<int> &buffer);


int main(int argc, char *argv[])
{
    int ntasks, rank;
    std::vector<int> sendbuf(2 * NTASKS), recvbuf(2 * NTASKS);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (ntasks != NTASKS) {
        if (rank == 0) {
            fprintf(stderr, "Run this program with %i tasks.\n", NTASKS);
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }


    /* Initialize message buffers */
    init_buffers(sendbuf, recvbuf);
    /* Print data that will be sent */
    print_buffers(sendbuf);
    /* Print data that was received */
    print_buffers(recvbuf);

    if (rank == 0) printf("Case 1:\n");
    if (rank == 0) recvbuf = sendbuf;
    MPI_Bcast(recvbuf.data(), recvbuf.size(), MPI_INT, 0, MPI_COMM_WORLD);
    print_buffers(recvbuf);

    if (rank == 0) printf("Case 2:\n");
    init_buffers(sendbuf, recvbuf);
    MPI_Scatter(sendbuf.data(), sendbuf.size() / ntasks, MPI_INT,
                recvbuf.data(), recvbuf.size() / ntasks, MPI_INT,
                0, MPI_COMM_WORLD); 
    print_buffers(recvbuf);

    if (rank == 0) printf("Case 3:\n");
    init_buffers(sendbuf, recvbuf);
    int counts[NTASKS] = {1, 1, 2, 4};
    int displs[NTASKS] = {0, 1, 2, 4};
    MPI_Gatherv(sendbuf.data(), counts[rank], MPI_INT,
                recvbuf.data(), counts, displs, MPI_INT,
                1, MPI_COMM_WORLD);
    print_buffers(recvbuf);

    if (rank == 0) printf("Case 4:\n");
    init_buffers(sendbuf, recvbuf);
    MPI_Alltoall(sendbuf.data(), sendbuf.size() / ntasks, MPI_INT,
                 recvbuf.data(), recvbuf.size() / ntasks, MPI_INT,
                 MPI_COMM_WORLD);
    print_buffers(recvbuf);

    MPI_Finalize();
    return 0;
}


void init_buffers(std::vector<int> &sendbuffer, std::vector<int> &recvbuffer)
{
    int rank;
    int buffersize = sendbuffer.size();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < buffersize; i++) {
        recvbuffer[i] = -1;
        sendbuffer[i] = i + buffersize * rank;
    }
}


void print_buffers(std::vector<int> &buffer)
{
    int rank, ntasks;
    int buffersize = buffer.size();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    std::vector<int> printbuffer(buffersize * ntasks);

    MPI_Gather(buffer.data(), buffersize, MPI_INT,
               printbuffer.data(), buffersize, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int j = 0; j < ntasks; j++) {
            printf("Task %2i:", j);
            for (int i = 0; i < buffersize; i++) {
                printf(" %2i", printbuffer[i + buffersize * j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

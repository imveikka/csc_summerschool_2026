/*
 * SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define NDIMS 2

int main(int argc, char* argv[]) {
    int ntasks, rank, irank;
    int dims[NDIMS] = {0};      /* Dimensions of the grid */
    int coords[NDIMS] = {0};    /* Coordinates in the grid */
    int neighbors[4] = {0}; /* Neighbors in 2D grid */
    int period[NDIMS] = {1, 1};
    MPI_Comm comm2d;        /* Cartesian communicator */
    int crank;              /* MPI rank in the Cartesian communicator */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Determine the process grid (dims[0] x dims[1] = ntasks) */
    // if (ntasks < 16) {
    //     dims[0] = 2;
    // } else if (ntasks >= 16 && ntasks < 64) {
    //     dims[0] = 4;
    // } else if (ntasks >= 64 && ntasks < 256) {
    //     dims[0] = 8;
    // } else {
    //     dims[0] = 16;
    // }
    // dims[1] = ntasks / dims[0];
    MPI_Dims_create(ntasks, NDIMS, dims);

    if (dims[0] * dims[1] != ntasks) {
        fprintf(stderr, "Incompatible dimensions: %i x %i != %i\n",
                dims[0], dims[1], ntasks);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    /* Create the 2D Cartesian communicator */
    MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, period, 1, &comm2d);

    /* Find out and store the neighboring ranks */
    MPI_Comm_rank(comm2d, &crank);
    MPI_Cart_shift(comm2d, 0, 1, neighbors, neighbors + 1);
    MPI_Cart_shift(comm2d, 1, 1, neighbors + 2, neighbors + 3);

    /* Find out and store also the Cartesian coordinates of a rank */
    MPI_Cart_coords(comm2d, crank, NDIMS, coords);

    for (irank = 0; irank < ntasks; irank++) {
        if (crank == irank) {
            printf("%3i = %2i %2i neighbors=%3i %3i %3i %3i\n",
                   crank, coords[0], coords[1], neighbors[0], neighbors[1],
                   neighbors[2], neighbors[3]);
        }
        MPI_Barrier(comm2d);
    }

    MPI_Finalize();
    return 0;
}

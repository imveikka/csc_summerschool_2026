/*
 * SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);

        int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int process_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	int last = world_size-1;
	
	printf("I'm process %d.\n", process_id);
	if (process_id == 0)
		printf("Total number of processes: %d.\n", world_size);
	if (process_id == (world_size - 1))
		printf("I'm the last but not least!\n");
	if (process_id == 42)
		printf("I'm the Answer to the Ultimate Question of Life, the Universe, and Everything!\n");

	MPI_Finalize();
}

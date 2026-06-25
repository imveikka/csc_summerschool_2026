// SPDX-FileCopyrightText: 2019 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cmath>
#include <mpi.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

constexpr int n = 1024;

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int extra = n % size; 
  	int istart = (n / size) * rank + 1 + MIN(rank, extra);
  	int istop = (n / size) * (rank + 1) + MIN(rank + 1, extra);
	
	printf("Computing approximation to pi with from i=%d to %d.\n", istart, istop);

  	double pi = 0.0;
  	for (int i = istart; i <= istop; i++) {
  	  	double x = (i - 0.5) / n;
  	  	pi += 1.0 / (1.0 + x*x);
  	}
  	pi *= 4.0 / n;

    // Reduce to rank 0
    // double total;
    // MPI_Reduce(&pi, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // if (rank == 0) printf("Approximate pi=%18.16f (exact pi=%10.8f).\n", total, M_PI);

    // Reduce to all
    MPI_Allreduce(MPI_IN_PLACE, &pi, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (rank == 0) printf("Approximate pi=%18.16f (exact pi=%10.8f).\n", pi, M_PI);


	MPI_Finalize();
}

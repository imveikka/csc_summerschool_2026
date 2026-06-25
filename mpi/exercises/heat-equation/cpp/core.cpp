// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

// Main solver routines for heat equation solver

#include <mpi.h>

#include "heat.hpp"

// Exchange the boundary values
void exchange(Field& field, const ParallelData parallel)
{

    double* sbuf;
    double* rbuf;
    // TODO start: implement halo exchange

    // You can utilize the data() method of the Matrix class to obtain pointer
    // to element, e.g. field.temperature.data(i, j)

    // Send to up, receive from down
    sbuf = &field.temperature(1, 1);
    rbuf = &field.temperature(field.nx+1, 1);
    MPI_Sendrecv(sbuf, field.ny, MPI_DOUBLE, parallel.nup, 0,
                 rbuf, field.ny, MPI_DOUBLE, parallel.ndown, MPI_ANY_TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Send to down, receive from up
    sbuf = &field.temperature(field.nx, 1);
    rbuf = &field.temperature(0, 1);
    MPI_Sendrecv(sbuf, field.ny, MPI_DOUBLE, parallel.ndown, 0,
                 rbuf, field.ny, MPI_DOUBLE, parallel.nup, MPI_ANY_TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // TODO end
}

// Update the temperature values using five-point stencil */
void evolve(Field& curr, const Field& prev, const double a, const double dt)
{

  // Compilers do not necessarily optimize division to multiplication, so make it explicit
  auto inv_dx2 = 1.0 / (prev.dx * prev.dx);
  auto inv_dy2 = 1.0 / (prev.dy * prev.dy);

  // Determine the temperature field at next time step
  // As we have fixed boundary conditions, the outermost gridpoints
  // are not updated.
  for (int i = 1; i < curr.nx + 1; i++) {
    for (int j = 1; j < curr.ny + 1; j++) {
            curr(i, j) = prev(i, j) + a * dt * (
	       ( prev(i + 1, j) - 2.0 * prev(i, j) + prev(i - 1, j) ) * inv_dx2 +
	       ( prev(i, j + 1) - 2.0 * prev(i, j) + prev(i, j - 1) ) * inv_dy2
               );
    }
  }

}


void exchange_evolve(Field& curr, Field& prev, const ParallelData parallel,
                     const double a, const double dt) {

    // Compilers do not necessarily optimize division to multiplication, so make it explicit
    auto inv_dx2 = 1.0 / (prev.dx * prev.dx);
    auto inv_dy2 = 1.0 / (prev.dy * prev.dy);

    // Initiate communications
    MPI_Request requests[4];
    double* sbuf;
    double* rbuf;

    // Send to up, receive from down
    sbuf = &prev(1, 1);
    rbuf = &prev(prev.nx + 1, 1);
    MPI_Isend(sbuf, prev.ny, MPI_DOUBLE, parallel.nup, 0,
              MPI_COMM_WORLD, requests);
    MPI_Irecv(rbuf, prev.ny, MPI_DOUBLE, parallel.ndown, MPI_ANY_TAG,
              MPI_COMM_WORLD, requests + 1);

    // Send to down, receive from up
    sbuf = &prev(prev.nx, 1);
    rbuf = &prev(0, 1);
    MPI_Isend(sbuf, prev.ny, MPI_DOUBLE, parallel.ndown, 0,
              MPI_COMM_WORLD, requests + 2);
    MPI_Irecv(rbuf, prev.ny, MPI_DOUBLE, parallel.nup, MPI_ANY_TAG,
              MPI_COMM_WORLD, requests + 3);

    // Compute inner values
    for (int i = 2; i < curr.nx; i++) {
        for (int j = 2; j < curr.ny; j++) {
                curr(i, j) = prev(i, j) + a * dt * (
    	       ( prev(i + 1, j) - 2.0 * prev(i, j) + prev(i - 1, j) ) * inv_dx2 +
    	       ( prev(i, j + 1) - 2.0 * prev(i, j) + prev(i, j - 1) ) * inv_dy2
                   );
        }
    }

    MPI_Waitall(4, requests, MPI_STATUS_IGNORE);

    // Compute boundaries
    const int I[4] = {1, 1, curr.nx, curr.nx};
    const int J[4] = {1, curr.ny, 1, curr.ny};
    for (int ix = 0; ix < 4; ix++) {
        int i = I[ix];
        for (int j = 1; j < curr.ny + 1; j++) {
                curr(i, j) = prev(i, j) + a * dt * (
    	       ( prev(i + 1, j) - 2.0 * prev(i, j) + prev(i - 1, j) ) * inv_dx2 +
    	       ( prev(i, j + 1) - 2.0 * prev(i, j) + prev(i, j - 1) ) * inv_dy2
                   );
        }
    }
    for (int ix = 0; ix < 4; ix++) {
        int j = J[ix];
        for (int i = 1; i < curr.nx + 1; i++) {
                curr(i, j) = prev(i, j) + a * dt * (
    	       ( prev(i + 1, j) - 2.0 * prev(i, j) + prev(i - 1, j) ) * inv_dx2 +
    	       ( prev(i, j + 1) - 2.0 * prev(i, j) + prev(i, j - 1) ) * inv_dy2
                   );
        }
    }

}


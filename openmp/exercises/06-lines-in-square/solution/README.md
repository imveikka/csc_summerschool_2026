<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion


## Task: Parallelize with OpenMP threads

1. Running the code with default arguments uses a random seed:

       Samples: 10000000
       Seed: 1499684558
       Thread   0: A few random values: 0.2505 0.3620 0.9848
       Average distance: 0.521416
       Calculation took 232.703 milliseconds

   The output with a fixed seed (0):

       Samples: 10000000
       Seed: 0
       Thread   0: A few random values: 0.0396 0.9921 0.1598
       Average distance: 0.521372
       Calculation took 232.494 milliseconds


2. **C++**

   See `lines-wrong.cpp` for a first attempt.
   Possible output with four threads:

       Samples: 10000000
       Seed: 0
       Thread   0: A few random values: 0.7317 0.4409 0.7829
       Thread   3: A few random values: 0.7317 0.4409 0.7829
       Thread   1: A few random values: 0.7317 0.4409 0.7829
       Thread   2: A few random values: 0.3266 0.5847 0.4754
       Average distance: 0.521395
       Calculation took 103.624 milliseconds

   We see that there is an issue with the random number sampling
   as multiple threads are getting the same random values.

   For a working solution, see `lines.cpp`. Output:

       Samples: 10000000
       Seed: 0 + thread number
       Thread   1: A few random values: 0.4512 0.1364 0.1339
       Thread   0: A few random values: 0.0396 0.9921 0.1598
       Thread   3: A few random values: 0.5902 0.1958 0.5588
       Thread   2: A few random values: 0.7838 0.8502 0.9036
       Average distance: 0.521431
       Calculation took 65.212 milliseconds

   The C++ code uses `omp_get_thread_num()` to set a different
   seed for each thread in order to create a different series
   of random numbers in each thread.

   **Note:** While this resolves the immediate issue of identical sequences,
   it is not a perfect solution. Using only slightly different seeds
   may still lead to subtle correlations between the generated sequences,
   depending on the random number generator. For demanding applications
   this could impact the statistical quality of the result.
   In such cases, more robust parallel random number generation techniques
   should be used.

   **Note:** This approach makes the final result depend on
   the number of threads. If reproducibility is required
   (e.g. identical results regardless of thread count),
   additional care is needed, typically at the cost of increased
   implementation complexity or reduced performance.

   **Fortran**

   See `lines.F90`.

   **Note:** Fortran's `random_number` shares a global state in a thread-safe way,
   so the C++ race condition does not occur.

   However, this also means that the random number generation becomes
   a serialization point and significantly limits parallel performance.
   To achieve good performance, proper parallel number generation should be used.

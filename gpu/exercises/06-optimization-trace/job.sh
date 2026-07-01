#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolGPU
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --gpus-per-node=1
#SBATCH --mem-per-cpu=32G
#SBATCH --time=00:05:00

# Enable GPU-aware MPI by uncommenting the line below
#export MPICH_GPU_SUPPORT_ENABLED=1

# Run the program
# srun ./a.out
srun rocprof --hip-trace ./a.out

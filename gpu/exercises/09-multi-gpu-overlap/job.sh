#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolGPU
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --gpus-per-node=1
#SBATCH --time=00:05:00

# Enable OpenMP offload debugging on LUMI by uncommenting the line below
export CRAY_ACC_DEBUG=1
export MPICH_GPU_SUPPORT_ENABLED=1

# Run the program
#srun ./select_gpu ./a.out $@
#srun ./jacobi $@
srun rocprofv3 -r --output-format=pftrace -- ./jacobi

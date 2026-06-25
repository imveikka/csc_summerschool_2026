#!/bin/bash
#SBATCH --job-name=mpi_06
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:01:00
#SBATCH --output=slurm-%x.out 

# Run the program
CC pi.cpp -o main
srun ./main

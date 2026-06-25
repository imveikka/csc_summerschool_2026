#!/bin/bash
#SBATCH --job-name=mpi_05
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:01:00
#SBATCH --output=slurm-%x.out 

# Run the program
CC chain.cpp -o main
srun ./main

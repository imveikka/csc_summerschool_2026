#!/bin/bash
#SBATCH --job-name=heat
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:01:00
#SBATCH --output=slurm-%x-%J.out 

# Run the program
srun ./build/heat_mpi bottle.dat 1000

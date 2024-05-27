#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=2
#SBATCH -o MPI/MPI2.txt
#SBATCH -e MPI/MPI2Errors.txt
mpirun MPI/MPI2 $1

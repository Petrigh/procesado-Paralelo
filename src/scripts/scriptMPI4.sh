#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=4
#SBATCH -o MPI/MPI4.txt
#SBATCH -e MPI/MPI4Errors.txt
mpirun MPI/MPI4 $1

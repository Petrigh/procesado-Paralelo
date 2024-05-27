#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o MPI/MPI8.txt
#SBATCH -e MPI/MPI8Errors.txt
mpirun MPI/MPI8 $1

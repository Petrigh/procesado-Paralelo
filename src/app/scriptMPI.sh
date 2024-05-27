#!/bin/bash
#SBATCH -N 1
#SBATCH –-exclusive
#SBATCH --partition=Blade
#SBATCH -o MPI/MPI
#SBATCH -e MPI/MPIErrors.txt
./secuencial.c $1 $2
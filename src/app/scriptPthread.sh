#!/bin/bash
#SBATCH -N 1
#SBATCH –-exclusive
#SBATCH --partition=XeonPHI
#SBATCH -o Pthread/Pthread
#SBATCH -e Pthread/PthreadErrors.txt
./secuencial.c $1 $2
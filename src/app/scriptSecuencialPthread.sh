#!/bin/bash
#SBATCH -N 1
#SBATCH –-exclusive
#SBATCH --partition=XeonPHI
#SBATCH -o Secuencial/secuencialPthread
#SBATCH -e Secuencial/secuencialPthreadErrors.txt
./secuencial.c $1
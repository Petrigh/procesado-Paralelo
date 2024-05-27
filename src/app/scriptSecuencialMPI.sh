#!/bin/bash
#SBATCH -N 1
#SBATCH –-exclusive
#SBATCH --partition=Blade
#SBATCH -o Secuencial/secuencialMPI
#SBATCH -e Secuencial/secuencialMPIErrors.txt
./secuencial.c $1
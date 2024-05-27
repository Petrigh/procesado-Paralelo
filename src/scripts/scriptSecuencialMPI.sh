#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=1
#SBATCH -o Secuencial/secuencialMPI.txt
#SBATCH -e Secuencial/secuencialMPIErrors.txt
mpirun Secuencial/secuencialMPI $1

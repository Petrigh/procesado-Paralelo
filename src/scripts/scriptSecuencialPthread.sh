#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=XeonPHI
#SBATCH -o Secuencial/secuencialPthread.txt
#SBATCH -e Secuencial/secuencialPthreadErrors.txt
source $ONEAPI_PATH/setvars.sh > /dev/null 2>&1
icc -pthread -o  Secuencial/SecuencialPthread Secuencial/secuencial.c
./Secuencial/SecuencialPthread $1

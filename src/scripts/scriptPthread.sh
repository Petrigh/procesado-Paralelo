#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=XeonPHI
#SBATCH -o Pthread/Pthread.txt
#SBATCH -e Pthread/PthreadErrors.txt
source $ONEAPI_PATH/setvars.sh > /dev/null 2>&1
icc -pthread -o Pthread/Pthread Pthread/pthread.c
./Pthread/Pthread $1 $2

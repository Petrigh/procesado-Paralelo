# Parallel Algorithms

Parallel algorithms in C for numeric sorting using multithreading techniques in distributed and shared memory systems

## Overview
This project consists of 2 optimized algorithms that sort 2 arrays of numbers in ascending order and then checks if they contain the same numbers.
Each algorithm tackles the challenge with a different architecture in mind, one with a shared memory system using Pthreads, and the other in a distributed memory enviroment, where MPI was used.
Both algorithms were run and tested in a cluster located in UNLP's computer science faculty.

## Getting Started

- [Running the algorithms](#running)
- [LICENSE](#license)

### Running the algorithms
<a name="running"></a>

All algorithms are located in the src/app/ direcotry.

#### Pthread

For running the shared memory architecture algorithm, the gcc compiler is needed, it already comes with pthread directives, so theres no need for extra package installations.
We compile our code in the bin/ folder and we call the output 'pthread'

```
  $ gcc -pthread -o bin/pthread src/app/pthread.c
```

Once compiled, 2 parameters must be passed to run the code, the size of the array will be 2 to the power of the number passed as the first parameter, while the second parameter is the number of threads used to solve the problem.

```
  $ ./bin/pthread <PowerOf2> <Threads>
```

#### MPI


For running the distributed memory architecture algorithm, the mpicc compiler is needed.
We compile our code in the bin/ folder and we call the output 'mpi'

```
  $ mpicc -o bin/mpi src/app/pthread.c
```

Once compiled, 1 parameter must be passed to run the code, the size of the array will be 2 to the power of the parameter.
To run it, a configuration file is needed where one assing the number of nodes used in the cluster.

### LICENSE
<a name="license"></a>

MIT License

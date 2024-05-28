#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*_________PROTOTYPES___________*/
double dwalltime();
void* inicializar();
void* mergesort(int*, int);
void* sort(int*, int, int);
int comparar(int*, int*, int);

/*__________MAIN____________*/
int main(int argc, char** argv){
    int miID; // ID de cada proceso
    int nrProcesos; //Cantidad de procesos
    int N = 0; // Tamaño de los arreglo (2^N)
    int *A = NULL; // Arreglo A (primer arreglo)
    int *B = NULL; // Arreglo B (segundo arreglo)
    int *parteA = NULL; // Parte del arreglo A que le corresponde a cada proceso
    int *parteB = NULL; // Parte del arreglo B que le corresponde a cada proceso
    int *comparaciones = NULL; // Arreglo que recorre las comparaciones de todos los procesos
    int comparacion = 1; // Valor de comparacion de cada proceso
    double timetick;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &miID);
    MPI_Comm_size(MPI_COMM_WORLD, &nrProcesos);

    
    if (nrProcesos % 2 != 0){ // Verifica si el numero de procesos es par
        fprintf(stderr, "El numero de procesos debe de ser par");
        MPI_Finalize();
        return 1;
    }

    if (argc != 2){ // Verifica si se obtuvo el tamaño del arreglo por parametro
        if (miID == 0){
            fprintf(stderr, "Falta definir el tamaño del arreglo");
        }
        MPI_Finalize();
        return 1;
    } else {
        N = 1<<atoi(argv[1]);
    }
    

    // Defino los arreglos locales a cada proceso
    parteA = (int*)malloc(sizeof(int)*N/nrProcesos);
    parteB = (int*)malloc(sizeof(int)*N/nrProcesos);
    

    /*
    // Asignar suficiente memoria para la fase más grande
    int maxPhaseSize = N / nrProcesos * (1 << (atoi(argv[1])));
    parteA = (int*)malloc(sizeof(int) * maxPhaseSize);
    parteB = (int*)malloc(sizeof(int) * maxPhaseSize);
    */

    if (miID == 0){
        // Defino los arreglos a comparar y el arreglo de las comparaciones
        A = (int*)malloc(sizeof(int)*N);
        B = (int*)malloc(sizeof(int)*N);
        comparaciones = (int*)malloc(sizeof(int)*nrProcesos);

        srand(time(NULL));

        inicializar(A,B,N); // Inicializo los dos arreglos
        
        B[N-1] = A[0];// Modifico un elemento de los arreglos para hacerlos diferentes
        
        timetick = dwalltime(); // Empiezo el calculo del tiempo en ejecutar
    }

    int maxFases = atoi(argv[1])+1;
    int counts[nrProcesos];
    int displacements[nrProcesos];

    for(int i= 0; i<nrProcesos;i++){
        displacements[i] = (N/nrProcesos)*i;
    }

    for(int k=0;k<maxFases;k++){
        for(int j=0; j<nrProcesos; j++){
            if(j%(1<<k)){
                counts[j] = 0;
            }else{
                counts[j] = (N/nrProcesos)*(1<<k);
            }
        }

        if (counts[miID] == 0){
            parteA = (int*)realloc(parteA, sizeof(int)*(N/nrProcesos)*(1<<k));
            parteB = (int*)realloc(parteB, sizeof(int)*(N/nrProcesos)*(1<<k));
        }
        /*
        A[N] = { X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X ,X, X, X, X, X }
                |  proceso 0  |  proceso 1  |  proceso 2  |  proceso 3  |  proceso 4  |  proceso 5  |  proceso 6  | proceso 7 |

        counts = { 5, 5, 5, 5, 5, 5, 5, 5 }
        displacement = { 0, 5, 10 15, 20, 25, 30, 35, 40}

        A[N] = { X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X ,X, X, X, X, X, X, X }
                |  proceso 0                                            |  proceso 4                                                |

        counts = { 20, 0, 0, 0, 20, 0, 0, 0, 0 }
        displacement = { 0, X, X, X, 20, X, X, X, }
        */
        
        // Se distribuye los arreglos entre los procesos
        MPI_Scatterv(A, counts, displacements, MPI_INT, parteA, (N/nrProcesos)*(1<<k), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(B, counts, displacements, MPI_INT, parteB, (N/nrProcesos)*(1<<k), MPI_INT, 0, MPI_COMM_WORLD);

        // Ordena cada arreglo asignado
        /*
        mergesort(parteA, (N/nrProcesos)*(1<<k));
        mergesort(parteB, (N/nrProcesos)*(1<<k));
        */
        mergesort(parteA, counts[miID]);
        mergesort(parteB, counts[miID]);
        

        // Se obtiene cada parte de cada arreglo ordenado y se adjunta a los arreglos originales
        MPI_Gatherv(parteA, (N/nrProcesos)*(1<<k), MPI_INT, A, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gatherv(parteB, (N/nrProcesos)*(1<<k), MPI_INT, B, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    /*
    parteA = (int*)realloc(parteA, sizeof(int)*(N/nrProcesos));
    parteB = (int*)realloc(parteB, sizeof(int)*(N/nrProcesos));
    MPI_Scatter(A, N/nrProcesos, MPI_INT, parteA, (N/nrProcesos), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, N/nrProcesos, MPI_INT, parteB, (N/nrProcesos), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(comparaciones, nrProcesos, MPI_INT, comparacion, 1, MPI_INT, 0, MPI_COMM_WORLD);

    comparacion = comparar(parteA, parteB, (N/nrProcesos));

    MPI_Gather(comparacion, 1, MPI_INT, comparaciones, nrProcesos, MPI_INT, 0, MPI_COMM_WORLD);
    */
    if ( miID == 0){
        /*
        comparacion = 1;
        for(int i = 0; i < nrProcesos; i++){
            if (comparaciones[i] == 0){
                comparacion = 0;
                break;
            }
        } */
        printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);
        /*
        if(comparacion)
            printf("Los arreglos son iguales\n");
        else
            printf("Los arreglos son distintos\n");
        */
        free(A);
        free(B);
        free(comparaciones);
    }   
    
    free(parteA);
    free(parteB);

    MPI_Finalize();
    return 0;
}

/*_________TIME____________*/ 
#include <time.h>
#include <sys/time.h>
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

/*____________INITIALIZER_____________*/
void* inicializar(int* A, int* B, int N){
    // Asigno un elemento aleatorio al primer arreglo (A)
    for(int i = 0; i < N; i++){
        A[i] = rand() % 1200;
        B[i] = A[i];
    }
}

/*_________________MERGE SORT___________________*/
void* mergesort(int* array, int N){
    int actual;
    int comienzo;
    for (actual = 1; actual <= N-1; actual = 2*actual){ //Arma paquetes de 2; 4; ...; n/2 elementos
        for (comienzo = 0; comienzo <= N-1; comienzo += 2*actual){ //Ordena el paquete
            sort(array, comienzo, comienzo+actual);
        }
    }
}

void* sort(int* array, int left, int right){
    int middle = ((right - left)/2) + left;
    int i=left, j=middle, k=0;
    int temp[right-left];
    while(i<middle && j<right){
        if(array[i] <= array[j])
            temp[k++] = array[i++];
        else
            temp[k++] = array[j++];
    }
    while (i < middle)
        temp[k++] = array[i++];
    while(j < right)
        temp[k++] = array[j++];
    k--;
    while(k >= 0) {
        array[left + k] = temp[k];
        k--;
    }
}

/*__________________COMPARE______________________*/
int comparar(int* A, int* B, int N){
    int C = 1;
    for(int i=0; i < N; i++){
        if(A[i] != B[i]){
            C = 0;
            break;
        }
    }
    return C;
}
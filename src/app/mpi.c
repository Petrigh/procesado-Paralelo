#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*_________PROTOTYPES___________*/
double dwalltime();
void* inicializar();
void* mergesort(int*, int*, int);
void* merge(int*, int*, int, int, int);
int comparar(int*, int*, int);

/*__________MAIN____________*/
int main(int argc, char** argv){
    int miID; // ID de cada proceso
    int nrProcesos; //Cantidad de procesos
    int N = 0; // Tamaño de los arreglo (2^N)
    int *A = NULL; // Arreglo A (primer arreglo)
    int *B = NULL; // Arreglo B (segundo arreglo)
    int *tempA = NULL; // Arreglo temporal utilizado para hacer el merge en el arreglo A
    int *tempB = NULL; // Arreglo temporal utilizado para hacer el merge en el arreglo B
    int comparacion_local = 1; // Valor de comparacion de cada proceso
    int comparacion_global = 1; // Valor final que tendra la comparacion
    double timetick; 
    int tamParte; // Definicion del tamaño que recibe cada parte
    int parteA; // Entero que se utiliza de desplazamiento para el arreglo A
    int parteB; // Entero que se utiliza de desplazamiento para el arreglo B
    int fase; // Entero por el cual se manejara el bucle de ordenamiento
    int dest; // Entero que define el ID destino a enviar la parte ordenada
    int tamRecv; // Entero que define el tamaño a ordenar en cada iteracion
    MPI_Status status;

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
    
    tamParte = N/nrProcesos;

    // Defino los arreglos a comparar
    A = (int*)malloc(sizeof(int)*N);
    B = (int*)malloc(sizeof(int)*N);

    // Defino los arreglos temporales a cada proceso para utilizar en el ordenamiento
    tempA = (int*)malloc(sizeof(int)*N);
    tempB = (int*)malloc(sizeof(int)*N); //Este puede ser opcional, capaz con un arreglo solo se pueda reutilizar

    parteA = tamParte;
    parteB = tamParte;

    if (miID == 0){

        srand(time(NULL));

        inicializar(A,B,N); // Inicializo los dos arreglos
        
        timetick = dwalltime(); // Empiezo el calculo del tiempo en ejecutar
    }

    // Empiezo con el arreglo A

    // El proceso root (0) distribuye equitativamente una parte del arreglo A a cada proceso, incluyendose 
    MPI_Scatter(A, tamParte, MPI_INT, A, tamParte, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso ordena su parte
    mergesort(A, tempA, tamParte); 

    
    // Empiezo a hacer la comunicacion entre los procesos y voy ordenando
    for (fase = 1; fase < nrProcesos; fase = fase << 1) {
        tamRecv = tamParte * fase;
        if (miID % (fase << 1) == 0) {
            if (miID + fase < nrProcesos) {
                
                // Espera a recibir la parte del arreglo ordenado
                MPI_Recv(A + parteA, tamRecv, MPI_INT, miID + fase, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Fusionar los datos recibidos con los datos locales
                merge(A, tempA, 0, parteA, parteA + tamRecv);
                
                printf("Ordeno la parte recibida de %d en el arreglo A con mi id = %d: \n",miID+fase,miID);
                for (int i = 0; i < N; i++) {
                    printf("|%d ", A[i]);
                }
                printf("\n");
                // Aumenta el desplazamiento
                parteA += tamRecv;
            }
        } else {
            // Calculo el id destinatario
            dest = miID - fase;
            // Envio la parte arreglaada y salgo del bucle
            MPI_Send(A, tamRecv, MPI_INT, dest, 0, MPI_COMM_WORLD);
            break;
        }
    }

    
    // Espero a que terminen todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    // Sigo ordenando ahora el arreglo B

    // El proceso root (0) distribuye equitativamente una parte del arreglo B a cada proceso, incluyendose 
    MPI_Scatter(B, tamParte, MPI_INT, B, tamParte, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso ordena su parte
    mergesort(B, tempB, tamParte); 

    // Empiezo a hacer la comunicacion entre los procesos y voy ordenando
    for (fase = 1; fase < nrProcesos; fase = fase << 1) {
        tamRecv = tamParte * fase;
        if (miID % (fase << 1) == 0) {
            if (miID + fase < nrProcesos) {

                // Espera a recibir la parte del arreglo ordenado
                MPI_Recv(B + parteB, tamRecv, MPI_INT, miID + fase, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Fusionar la parte recibida con los datos locales
                merge(B, tempB, 0, parteB, parteB + tamRecv);
                
                // Aumenta el desplazamiento
                parteB += tamRecv;
            }
        } else {
            // Calculo el id destinatario
            dest = miID - fase;
            // Envio la parte arreglaada y salgo del bucle
            MPI_Send(B, tamRecv, MPI_INT, dest, 0, MPI_COMM_WORLD);
            break;
        }
    }

    // Espero a que terminen todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    // Ahora hago la comparacion
    //Distribuyo todos los arreglos a cada proceso
    MPI_Scatter(A, tamParte, MPI_INT, A, tamParte, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, tamParte, MPI_INT, B, tamParte, MPI_INT, 0, MPI_COMM_WORLD);
    // Cada proceso hace su parte de la comparacion
    comparacion_local = comparar(A, B, tamParte);
    
    // El proceso root recibe los valores y dictamina si son iguales o no a traves de un AND lógico (si todos son 1, es verdadero; caso contrario es falso)
    MPI_Reduce(&comparacion_local, &comparacion_global, 1, MPI_INT, MPI_LAND, 0, MPI_COMM_WORLD);

    // El proceso root (0) reúne los datos ordenados finales
    if (miID == 0) {
        printf("\n");
        printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);
        
    }

    free(A);
    free(B);
    free(tempA);
    free(tempB);
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
    // Asigno un elemento aleatorio al primer arreglo (A) y se lo copio hacia el segundo arreglo (B) asi son iguales
    for(int i = 0; i < N; i++){
        A[i] = rand() % 1200;
        B[N - 1 - i] = A[i];
    }
    //B[0] = A[0];// Modifico un elemento de los arreglos para hacerlos diferentes
}


/*_________________MERGE SORT___________________*/

void* mergesort(int* array, int* temp, int N) {
    int actual, comienzo;
    for (actual = 1; actual <= N - 1; actual = 2 * actual) { // Arma paquetes de 2, 4, ... n/2 elementos
        for (comienzo = 0; comienzo < N - 1; comienzo += 2 * actual) { // Ordena el paquete
            int middle = comienzo + actual;
            int end = (comienzo + 2 * actual < N) ? (comienzo + 2 * actual) : N;
            if (middle < end) {
                merge(array, temp, comienzo, middle, end);
            }
        }
    }
}

void* merge(int* array, int* temp, int left, int middle, int right) {
    int i = left, j = middle, k = 0;

    while (i < middle && j < right) {
        if (array[i] <= array[j]) {
            temp[k++] = array[i++];
        } else {
            temp[k++] = array[j++];
        }
    }

    while (i < middle) {
        temp[k++] = array[i++];
    }

    while (j < right) {
        temp[k++] = array[j++];
    }

    for (i = left, k = 0; i < right; i++, k++) {
        array[i] = temp[k];
    }

}

/*__________________COMPARE______________________*/
int comparar(int* A, int* B, int N){
    for(int i=0; i < N; i++){
        if(A[i] != B[i]){
            return 0;
        }
    }
    return 1;
}



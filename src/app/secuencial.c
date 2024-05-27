#include <stdio.h>
#include <stdlib.h>

int* A;
int* B;
int N;
unsigned long length;

/* ATENCION PARA EJECUTAR EN EL CLUSTER

USAR EL COMANDO sbatch ./script.sh
si solo corro el comando ./script.sh va a correr el comando en el front
*/

double dwalltime();
void* inicializar(void);
void* finalizar(void);
void* mergesort(int*, int, int, int);
void* sort(int*, int, int);
int compare(int, int);

int main(int argc, char* argv[]){

    N = atoi(argv[1]); // 2^N
    length = (1<<N); //longitud arrays
    inicializar();
    
    double timetick = dwalltime();
    mergesort(A,0,length,2);
    mergesort(B,0,length,2);
    int compareResult = compare(0,length);

    printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);
    if(compareResult)
        printf("Los arreglos son distintos\n");
    else
        printf("Los arreglos son iguales\n");

    finalizar();
    return 0;
}
/*
|A______________| |B______________|
*/

/*__________________COMPARE______________________*/
int compare(int left, int right){
    for(int i=left;i<right;i++){
        if(A[i] != B[i]){
            return 1;
        }
    }
    return 0;
}
/*_________________MERGE SORT___________________*/
void* mergesort(int* array, int left, int right, int fase){
    int longitud = right-left;
    int floor, offset;
    for(offset=fase;offset<=longitud;offset=offset*2){ //arma paquetes de 2; 4; ...; n/2 elementos
        for(int floor=left;floor<right;floor=floor+offset){ //sorta el paquete
            sort(array, floor, floor+offset);
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


/*_______________INITIALIZER_________________*/
void* inicializar(void){
    int i;
    //alocamos matrices en heap
    A = (int*)malloc(sizeof(int)*(length));
    B = (int*)malloc(sizeof(int)*(length));
    //inicializamos A
    for(i=0;i<length;i++){
        A[i] = rand() % 1024;
        B[length-i-1] = A[i]; //simulo desordenados
    }
    //repetimos elementos (numero 10 => nuestra nota)
    A[0]= 10;
    A[length-1]= 10;
    B[0]= 10;
    B[length-1]= 10;
}

void* finalizar(void){
    free(A);
    free(B);
}

/*_____________TIME______________*/
#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* A;
int* B;
int N;
int T;
unsigned long length;

/* ATENCION PARA EJECUTAR EN EL CLUSTER

USAR EL COMANDO sbatch ./script.sh
si solo corro el comando ./script.sh va a correr el comando en el front
*/

double dwalltime();
void* initArray(void*);
void* inicializar(void);
void mergesort(int* array, int left, int right);
void sort(int*, int left, int right);
void* funcion(void *arg);

int main(int argc, char* argv[]){

    N = atoi(argv[1]); //longitud arrays
    T = atoi(argv[2]); //cantidad de threads
    int threads_ids[T];
    pthread_t misThreads[T];
    length = (1<<N);
    printf("Generando array de %lu elementos\n",length);
    inicializar();
    printf("A[] = { ");
    for(int i=0;i<length;i++){
        printf("%d; ",A[i]);
    }
    printf("}\n");

    printf("B[] = { ");
    for(int i=0;i<length;i++){
        printf("%d; ",B[i]);
    }
    printf("}\n");

    double timetick = dwalltime();

    for(int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&funcion,(void*)&threads_ids[id]);
    }

    for(int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }
    
    printf("A[] = { ");
    for(int i=0;i<length;i++){
        printf("%d; ",A[i]);
    }
    printf("}\n");
    printf("B[] = { ");
    for(int i=0;i<length;i++){
        printf("%d; ",B[i]);
    }
    printf("}\n");
    printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);

    free(A);
    free(B);
    return 0;
}


/*_________________MERGE SORT___________________*/
void mergesort(int* array, int left, int right){
    int longitud = right-left;
    int floor, offset;
    for(offset=2;offset<=longitud;offset=offset*2){
        for(int floor=0;floor<longitud;floor=floor+offset){
            sort(array, floor, floor+offset);
        }
    }
}

void sort(int* array, int left, int right){
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

/*__________SCHEDULER________________*/
void* funcion(void *arg){
    int tid=*(int*)arg;
    int inicio = (length/T)*tid;
    int limite = inicio+(length/T);
    printf("Hilo id:%d sorting A[] segment . . .\n", tid);
    mergesort(A,inicio,limite);
    printf("Hilo id:%d sorting B[]] segment . . .\n", tid);
    mergesort(B,inicio,limite);

    
    pthread_exit(NULL);
}


void* inicializar(void){
    int i;
    //alocamos matrices en heap
    A = (int*)malloc(sizeof(int)*(length));
    B = (int*)malloc(sizeof(int)*(length));
    //inicializamos A
    for(i=0;i<length;i++){
        A[i] = rand() % 1024;
        B[length-i-1] = A[i];
    }
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
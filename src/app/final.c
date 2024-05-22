#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* A;
int* B;
int N;
int T;
unsigned long length;
pthread_barrier_t* barreraMerge;
pthread_barrier_t barreraCompare;

/* ATENCION PARA EJECUTAR EN EL CLUSTER

USAR EL COMANDO sbatch ./script.sh
si solo corro el comando ./script.sh va a correr el comando en el front
*/

double dwalltime();
void* inicializar(void);
void mergesort(int* array, int left, int right);
void sort(int*, int left, int right);
void* divideAndConquer(void *arg);

int main(int argc, char* argv[]){

    N = 4;//atoi(argv[1]); //longitud arrays
    T = 4;//atoi(argv[2]); //cantidad de threads
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
        pthread_create(&misThreads[id],NULL,&divideAndConquer,(void*)&threads_ids[id]);
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
    for(offset=2;offset<=longitud;offset=offset*2){ //arma paquetes de 2; 4; ...; n/2 elementos
        for(int floor=left;floor<right;floor=floor+offset){ //sorta el paquete
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
void* divideAndConquer(void *arg){
    int tid=*(int*)arg;
    int inicio = ((length<<1)/T)*(tid%(T/2));
    int limite = inicio+(length/T);
    if(tid>(T/2)){

    }else{

    }

    printf("Hilo id:%d sorting B[]] segment . . .\n", tid);
    mergesort(A,inicio,limite);

    
    pthread_exit(NULL);
}

/*_______________INITIALIZER_________________*/
void* inicializar(void){
    int i;
    //alocamos matrices en heap
    A = (int*)malloc(sizeof(int)*(length));
    B = (int*)malloc(sizeof(int)*(length));
    barreraMerge = (pthread_barrier_t*)malloc(sizeof(pthread_barrier_t)*(T));
    //inicializamos A
    for(i=0;i<length;i++){
        A[i] = rand() % 1024;
        B[length-i-1] = A[i];
    }
    for(i=0;i<T;i++){
        barreraMerge[i]= pthread_barrier_init(&barreraMerge[i], NULL, 2);
    }
    barreraCompare = pthread_barrier_init(&barreraCompare, NULL, T);
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
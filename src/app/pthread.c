#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* A;
int* B;
int* TempA;
int* TempB;
int N;
int T;
int numThread;
unsigned long length;
pthread_barrier_t* barreraMerge;
pthread_barrier_t barreraCompare;
pthread_mutex_t compareMutex;
int compareResult = 0;

/* ATENCION PARA EJECUTAR EN EL CLUSTER

USAR EL COMANDO sbatch ./script.sh
si solo corro el comando ./script.sh va a correr el comando en el front
*/

double dwalltime();
void printArrays(int, int, int);
int inicializar(void);
void* finalizar(void);
void* mergesort(int*, int*, int, int, int);
void* sort(int*, int*, int, int);
int compare(int, int);
void* divideAndConquer(void*);

int main(int argc, char* argv[]){
    if (atoi(argv[1]) <= 0){
        printf("El tamano del array debe de ser mayor a 1\n");
        return 1;
    }

    if (atoi(argv[2]) > 0 && atoi(argv[2])%2){
        printf("El numero de procesos debe de ser par mayor a 0\n");
        return 1;
    }
    N = atoi(argv[1]); // 2^N
    T = atoi(argv[2]); //cantidad de threads   comment: revisar 8 o mas threads en cluster
    int threads_ids[T];
    pthread_t misThreads[T];
    numThread = T/2; //una mitad para A[] y la otra para B[]
    length = (1<<N); //longitud arrays
    if(inicializar()) //Inicializa los dos arreglos
        printf("Error al alocar memoria.\n");
    double timetick = dwalltime();

    for(int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&divideAndConquer,(void*)&threads_ids[id]);
    }

    for(int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }
    printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);

    if(compareResult)
        printf("Los arreglos son distintos\n");
    else
        printf("Los arreglos son iguales\n");

    finalizar();
    return 0;
}

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
void* mergesort(int* array, int* temp, int left, int right, int fase){
    int longitud = right-left;
    int floor, offset;
    for(offset=fase;offset<=longitud;offset=offset*2){ //arma paquetes de 2; 4; ...; n/2 elementos
        for(int floor=left;floor<right;floor=floor+offset){ //sorta el paquete
            sort(array, temp, floor, floor+offset);
        }
    }
}

void* sort(int* array, int* Temp, int left, int right){
    int middle = ((right - left)/2) + left;
    int i=left, j=middle, k=left;
    while(i<middle && j<right){
        if(array[i] <= array[j])
            Temp[k++] = array[i++];
        else
            Temp[k++] = array[j++];
    }
    while (i < middle)
        Temp[k++] = array[i++];
    while(j < right)
        Temp[k++] = array[j++];
    for(k=left;k<right;k++){
        array[k]=Temp[k];
    }
}

/*__________SCHEDULER________________*/
void* divideAndConquer(void *arg){
    int tid=*(int*)arg;
    int parte = length/numThread;
    int inicio, limite;
    int fase=0;
    int offset = 2;
    while(parte<=length){
        inicio = parte*((tid)%numThread);
        limite = parte*((tid)%numThread)+parte;

        if(tid<numThread){
            mergesort(A, TempA, inicio,limite,offset);
        }else{
            mergesort(B, TempB, inicio,limite,offset);
        }
        pthread_barrier_wait(&barreraMerge[(tid/(1<<(fase+1)))]);
        if (tid % (1 << (fase + 1)) != 0) {
            break; //los threads ociosos salen
        }
        (fase) ? (fase = fase<<1) : (fase = 1);
        offset = parte;
        parte = parte<<1;
    }
    pthread_barrier_wait(&barreraCompare);

    parte=length/T;
    inicio = parte*tid;
    limite = parte*tid+parte;
    if(compare(inicio,limite)){

        pthread_mutex_lock(&compareMutex);
        compareResult = 1;
        pthread_mutex_unlock(&compareMutex);
    }
    pthread_exit(NULL);
}

/*_______________INITIALIZER_________________*/
int inicializar(void){
    int i;
    //alocamos matrices en heap
    A = (int*)malloc(sizeof(int)*(length));
    B = (int*)malloc(sizeof(int)*(length));
    TempA = (int*)malloc(sizeof(int)*(length));
    TempB = (int*)malloc(sizeof(int)*(length));

    if (A == NULL || B == NULL || TempA == NULL || TempB == NULL) 
        return 1;
    barreraMerge = (pthread_barrier_t*)malloc(sizeof(pthread_barrier_t)*(T/2));
    //inicializamos A
    for(i=0;i<length;i++){
        A[i] = rand() % (length/2); //siempre habran repetidos
        B[length-i-1] = A[i]; //simulo desordenados
    }
    for(i=0;i<(T/2);i++){
        pthread_barrier_init(&barreraMerge[i], NULL, 2);
    }
    pthread_barrier_init(&barreraCompare, NULL, T);
    pthread_mutex_init(&compareMutex, NULL);
    return 0;
}

void* finalizar(void){
    pthread_mutex_destroy(&compareMutex);
    pthread_barrier_destroy(&barreraCompare);
    for(int i=0;i<(T/2);i++){
        pthread_barrier_destroy(&barreraMerge[i]);
    }
    free(A);
    free(B);
    free(TempA);
    free(TempB);
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

void printArrays(int start, int end, int length){

    printf("A[%d]={ ",length);
    for(int i=start;i<end-1;i++)
        printf("%d;",A[i]);
    printf("%d }\n",A[end-1]);

    printf("B[%d]={ ",length);
    for(int i=start;i<end-1;i++)
        printf("%d;",B[i]);
    printf("%d }\n",B[end-1]);

    printf("T[%d]={ ",length);
    for(int i=start;i<end-1;i++)
        printf("%d;",TempA[i]);
    printf("%d }\n\n",TempA[end-1]);

    printf("U[%d]={ ",length);
    for(int i=start;i<end-1;i++)
        printf("%d;",TempB[i]);
    printf("%d }\n\n",TempB[end-1]);
}
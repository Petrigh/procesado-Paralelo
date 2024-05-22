#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* A;
int* B;
int N;
int T;

double dwalltime();
void* inicializar(void);

void* funcion(void *arg){
    int tid=*(int*)arg;
    int inicio = (N/T)*tid;
    int limite = inicio+(N/T);
    int minimo = 1025;
    int maximo = -1;
    printf("Hilo id:%d\n",tid);

    for (int i = inicio; i < limite; i++) {
        if(A[i] < minimo){
            B[tid] = A[i];
            minimo = A[i];
        }else{
            if(A[i] > maximo){
                B[tid + T] = A[i];
                maximo = A[i];
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){

    N = atoi(argv[1]); //longitud arrays
    printf("2^N = %lu\n", (unsigned long)(1<<N));
    return 0;
    T = atoi(argv[2]); //cantidad de threads
    pthread_t misThreads[T];
    int threads_ids[T];

    inicializar();

    double timetick = dwalltime();
    for(int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&funcion,(void*)&threads_ids[id]);
    }

    int minimo = 1025;
    int maximo = -1;

    for(int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
        if(B[id] < minimo){
            minimo = B[id];
        }else{
            if(B[id+T] > maximo){
                maximo = B[id+T];
            }
        }
    }

    printf("B[minimos] = ");
    for (int i = 0; i < T; i++) {
        printf("%d ",B[i]);
    }
    printf("\nB[maximos] = ");
    for (int i = T; i < T*2; i++) {
        printf("%d ",B[i]);
    }

    printf("\nEl numero minimo es: %d\nEl numero maximo es:%d\n", minimo, maximo);
    printf("Tiempo de ejecucion: %fs \n", dwalltime() - timetick);

    free(A);
    free(B);

    return 0;
}


void* inicializar(void){
    //alocamos matrices en heap
    A = (int*)malloc(sizeof(int)*(2^N));
    B = (int*)malloc(sizeof(int)*T*2);
    //inicializamos A y B en 1
    printf("A[%d] = ",N);
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 1024;
        printf("%d ",A[i]);
    }
    printf("\n");
}

/*___________________________*/
#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
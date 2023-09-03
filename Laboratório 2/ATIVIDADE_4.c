/*
Programação Concorrente - Laboratório 2
Aluno: Lucas Cavalcante Nascimento Silva
DRE: 121056863
*/

//=======================================================================================

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include "timer.h"

int cont_primos = 0;
pthread_mutex_t mutex; //variavel de lock para exclusao mutua

int ehPrimo(long long int n) {
    int i;
        if(n<=1) return 0;
        if(n==2) return 1;
        if(n%2==0) return 0;
        for (i=3; i<sqrt(n+1); i=i+2)
            if(n%i==0) return 0;
        return 1;
}

// Estrutura de dados a ser passada como argumento na função paralelizada
typedef struct{
    int id;
    long long int inicio, fim;
    int passo;
} tArgs;

void * contaPrimo(void *arg){
    // Esta função dispara um fluxo de execução de checagem de primos,
    // alternando com um passo determinado
    tArgs *args = (tArgs*) arg; 

    int i;
    for( i=args->inicio ; i<args->fim ; i=i+args->passo){
        if(ehPrimo(i)){
        //Entrada na SC
        pthread_mutex_lock(&mutex);
        //--SC
        cont_primos++;
        //Saída da SC
        pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}



//=======================================================================================

int main(int argc, char* argv[]){
    int N, nthreads;
    pthread_t *tid; // identificadores das threads do sistema
    double inicio, fim, delta;
    long long int i;
    tArgs *args;

    //--Ajustando os parâmetros de execução do programa
    if(argc<3) {
        printf("Digite: %s <N> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    if(nthreads > N) nthreads = N;

    //--Inicializa o mutex 
    pthread_mutex_init(&mutex, NULL);
    GET_TIME(inicio); // Inicia o timer
    
    //--Preenchendo os argumentos e iniciando as threads:
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
    args = (tArgs*)malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {puts("ERRO--malloc"); return 2;}

    for ( i=0 ; i<nthreads ; i++){
        (args+i)->id = i;
        (args+i)->inicio = i+1;
        (args+i)->fim = N;
        (args+i)->passo = nthreads;

        if(pthread_create(tid+i, NULL, contaPrimo,(void*) (args+i))){
        puts("ERRO--pthread_create"); return 3;}
    }

    //--Espera pelo termino da threads
    for(int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }
    GET_TIME(fim);
    delta = fim - inicio;
    
    //teste:
    printf("N , %d , Threads , %d , ", N, nthreads);
    printf("TEMPO DECORRIDO , %lf , Primos , %d\n", delta, cont_primos);
    return cont_primos;
}
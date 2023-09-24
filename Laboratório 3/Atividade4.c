/* Programação Concorrente */
/* Aluno: Lucas Cavalcante N. Silva */
/* Laboratório 3 - Atividade 4 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* variáveis globais */
int barreira_1 = 0, barreira_2 = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond1, x_cond2;

/*=======================================================================================================*/

/* Thread A (Olá) */
void *A(int id){
    printf("Ola da thread %d\n", id);

    /* SC */
    pthread_mutex_lock(&x_mutex);
    barreira_1++;
    
    if(barreira_1==3){
        pthread_cond_broadcast(&x_cond1);
        printf("\n");
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Thread B (Que dia bonito) */
void *B(int id){

    pthread_mutex_lock(&x_mutex);
    if(barreira_1 < 3){
        pthread_cond_wait(&x_cond1, &x_mutex);
    }
    
    /* SC */
    printf("Que dia bonito %d\n", id);
    barreira_2++;
    if(barreira_2==3){
        pthread_cond_broadcast(&x_cond2);
        printf("\n");
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Thread C (Até breve) */
void *C(int id){

    pthread_mutex_lock(&x_mutex);
    if(barreira_2 < 3){
        pthread_cond_wait(&x_cond2, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);
    printf("Até breve da thread %d\n", id);
    pthread_exit(NULL);
}


/*=======================================================================================================*/

int main(int argc, char *argv[]){

    int nthreads;

    if(argc<2){
        printf("Digite: \"atividade4 <numero de threads>\"\n");
        return 1;
    }
    nthreads = atoi(argv[1]);

    /* Alocando espaço para as threads */
    pthread_t *threads;
    threads = (pthread_t *)malloc(sizeof(pthread_t)*nthreads*3);

    /* Inicializando o lock de exclusão mútua e as variáveis de condição */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond1, NULL);
    pthread_cond_init(&x_cond2, NULL);

    /* Instanciando função para cada thread */
    int i;
    for(i=0; i<nthreads; i++){
        pthread_create(&threads[i], NULL, A, i+1);
    }
    for(i=0; i<nthreads; i++){
        pthread_create(&threads[i+nthreads], NULL, B, i+1); 
    }
    for(i=0; i<nthreads; i++){
        pthread_create(&threads[i+nthreads*2], NULL, C, i+1);
    }

    /* Espera todas as threads terminarem */
    for(i=0;i<nthreads*3;i++){
        pthread_join(threads[i], NULL);
    }
    
    /* Desaloca variáveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond1);
    pthread_cond_destroy(&x_cond2);

    return 0;
}

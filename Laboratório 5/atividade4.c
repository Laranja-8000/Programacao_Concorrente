/*
    Programação Concorrente
    Aluno: Lucas Cavalcante Nascimento Silva
    DRE: 121056863
    Laboratório 5 - Atividade 4 
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5 // número de elementos no buffer

// Variaveis globais
sem_t slotCheio, slotVazio;  // semáforos para sincronização por condição
sem_t mutexGeral;            // semáforo geral de sincronização por exclusão mútua

char Buffer_[N][256]; // Buffer contendo N linhas
char *nome_arquivo;

//===============================================================================================================================
// Funções 

/* Função de inserção usada pela thread produtora. Copia uma linha de texto para o buffer. */
void Insere_(char item[], int id){
    static int in=0;
    sem_wait(&slotVazio);
    sem_wait(&mutexGeral);

    int i;
    for(i=0;i<256;i++){ // Loop para copiar linhas de texto.
      Buffer_[in][i] = item[i];
    }
    in = (in + 1) % N;
    sem_post(&mutexGeral);
    sem_post(&slotCheio);
}

/* Função de remoção usada pela thread consumidora.
  Exibe a linha a ser alterada e limpa o conteúdo na posição respectiva no buffer. */
void Retira_(int id){
    char item[256];
    static int out = 0;
    sem_wait(&slotCheio);
    sem_wait(&mutexGeral);

    int i;
    for(i=0; i<256;i++){ // Loop para copiar linhas de texto.
      item[i] = Buffer_[out][i];
    }
    Buffer_[out][0] = 0;
    out = (out + 1) % N;
    printf("Cons[%d]: retirou %s\n", id, item);
    sem_post(&mutexGeral);
    sem_post(&slotVazio);    
}


//----------------------------------------------------------
// Produtor
void *produtor_(void * arg) {
  int id = *(int *)(arg);
  free(arg);
  FILE *fp;
  char line[256];
  fp = fopen(nome_arquivo,"r");

  while( fgets(line, sizeof(line), fp)){
    Insere_(line, id);
  }

  pthread_exit(NULL);
}

//----------------------------------------------------------
// Consumidor
void *consumidor(void * arg) {
  int item, id = *(int *)(arg);
  free(arg);
  while(1) {
    Retira_(id);
    sleep(1);
  }
  pthread_exit(NULL);
}

//===============================================================================================================================
// Função principal
int main(int argc, char **argv) {
  pthread_t *tid, thread_produtora;
  int i, *id, nthreads;

  // Verifica se o número de threads e o nome do arquivo foi passado na linha de comando
  if(argc<3){
        printf("Digite: ./%s <numero de threads> <nome do arquivo>\n",argv[0]);
        return 1;
   }
  nthreads = atoi(argv[1]);
  nome_arquivo = argv[2];

  // Inicia os semaforos
  sem_init(&mutexGeral, 0, 1);
  sem_init(&slotCheio, 0, 0);
  sem_init(&slotVazio, 0, N);
  
  // Inicia a thread produtora
  id = (int *) malloc(sizeof(int));
  *id = i+1;
  if (pthread_create(&thread_produtora, NULL, produtor_, (void *) (id))) {
      printf("Erro na criacao do thread produtor\n");
      exit(1);
  }

  // Inicia as threads consumidoras
  tid = malloc(sizeof(pthread_t) * nthreads);
  for(i=0; i< nthreads; i++){
    id = (int *)malloc(sizeof(int));
    *id = i+1;
    if( pthread_create(tid+i, NULL, consumidor, (void *) (id))){
        printf("Erro na criacao da thread consumidora\n");
        exit(1);
    }
   }

  pthread_exit(NULL);
}        

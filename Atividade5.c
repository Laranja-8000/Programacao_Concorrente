/* Disciplina: Programação Concorrente */
/* Aluno: Lucas Cavalcante Nascimento Silva */
/* DRE: 121056863 */

// =====================================================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 8 // Total de threads a serem criadas
#define TAMANHO_VETOR 10000 // Tamanho de elementos do vetor

//--Cria a estrutura de dados para armazenar os argumentos da thread:
typedef struct{
    int *enderecoInt, N;
} t_Args;

//--Função implementada pelas threads:
void *CalculaQuadrado(void *arg){
    /* Esta função sobrescreve o valor referenciado no ponteiro p
     e avança para o endereço imediatamente próximo, N vezes. */
    t_Args *args = (t_Args *) arg;

    int i;
    for(i=0;i < args->N;i++){
        *(args->enderecoInt) = (*args->enderecoInt)*(*args->enderecoInt);
        args->enderecoInt = args->enderecoInt + 1; // Pula para o próximo endereço de memória
    }
    pthread_exit(NULL);
}


// ======================================================================================================================
//--Função principal do programa:
int main(){

    pthread_t tid_sistema[NTHREADS]; // Identificadores das threads no sistema
    t_Args *args; // Receberá os argumentos para a thread

    args = malloc(sizeof(t_Args) * NTHREADS);

    int Vetor[TAMANHO_VETOR];
    int i;
    // O vetor tem 10 000 elementos, logo é possível paralelizar o processo
    // em 8 threads se cada thread der conta de exatamente 1250 elementos.
    // Sendo assim, usaremos como argumentos:
    //      - O endereço *enderecoInt de uma variável inteira, a ser sobrescrita inicialmente;
    //      - Um número N de vezes que executaremos a operação de sobrescrever e prosseguir para
    //        a próxima posição, de onde repetiremos a operação;
    //

    int tamanho_fatia = TAMANHO_VETOR / NTHREADS; // A princípio, o Vetor será dividido em fatias de 1250 elementos, mas isto é ajustável


    //--Atribuindo valores aos elementos do vetor para teste:
    for(i=0;i<10000;i++){
        Vetor[i] = i % 10;
    }


    //--Atribuindo os valores aos argumentos a serem passados na criação das threads
    //  Como são 8 threads, no caso, teremos um 'vetor de argumentos', por assim dizer, de 8 elementos
    for(i=0 ; i < NTHREADS; i++){
        args[i].N = tamanho_fatia;
        args[i].enderecoInt = &Vetor[i * tamanho_fatia];
    }

    //--Criando as threads:
    for(i=0; i < NTHREADS; i++) {
        if (pthread_create(&tid_sistema[i], NULL, CalculaQuadrado, (void*) &args[i])) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //--Espera todas as threads terminarem:
    for (i=0; i < NTHREADS; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }
    printf("--Thread principal terminou\n");
  
    //--Verificando o resultado final após o término de todas as threads:
    for(i=0;i<10000;i++){
        printf("%d ", Vetor[i]);
    }

    pthread_exit(NULL);
    return 0;
}
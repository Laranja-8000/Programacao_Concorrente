#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

#define G 6.67e-11  // Constante gravitacional
#define DT 86400   // Passo de tempo
#define OUTPUT_FILE "output.txt"  // Nome do arquivo de saída
#define N_CORPOS 9 // Número de corpos

// Estrutura do args da tarefa
typedef struct{
    int inicio;
    int fim;
} Argumento;

// Estrutura de cada corpo
typedef struct {
    double x, y;     // Posição
    double vx, vy;   // Velocidade
    double mass;     // Massa
} Corpo;

// Variáveis globais
Corpo corpos[N_CORPOS];
double duration = 86400 * 365 * 10;
FILE *output_file;  // Arquivo

//variáveis barreira
const int nthreads = 6;
int contador = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//implementação da barreira
void barreira(){
    pthread_mutex_lock(&x_mutex);
    contador++;

    if(contador < nthreads){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    else{
        contador = 0;
        pthread_cond_broadcast(&x_cond);
    }

    pthread_mutex_unlock(&x_mutex);
}

void calculaPosicao(int q) {
    corpos[q].x += corpos[q].vx * DT;
    corpos[q].y += corpos[q].vy * DT;
}

void calculaVelocidade(int q, double ax, double ay) {
    corpos[q].vx += ax * DT;
    corpos[q].vy += ay * DT;
}

void calculaForca(int i, int j, double *ax, double *ay) {
    double dx = corpos[j].x - corpos[i].x;
    double dy = corpos[j].y - corpos[i].y;
    double distance = sqrt(dx * dx + dy * dy);
    double force = G * corpos[i].mass * corpos[j].mass / (distance * distance);

    *ax = force * dx / distance;
    *ay = force * dy / distance;
}

void write_positions_to_file(int q) {
    fprintf(output_file, "%.2f %.2f\n", corpos[q].x, corpos[q].y);
}

pthread_mutex_t mutexFile;

void *tarefa(void *arg){
    Argumento *args = (Argumento*) arg;
    int inicio = args->inicio;
    int fim = args->fim;

    int q = inicio;
    int i, i2, j;

    //itera sobre o tempo
    for (double t = 0; t < duration; t += DT) {
        //itera sobre cada partícula
        for (i = inicio; i <= fim; i++){
            q = i;
            // Escreve as posições
            write_positions_to_file(q);

            // Atualiza as posições
            calculaPosicao(q);
        }

        barreira();

        for (i2 = inicio; i2 <= fim; i2++){
            q = i2;

            // Calcula forças e atualiza as velocidades
            double ax = 0, ay = 0;
            
            for (j = 0; j < N_CORPOS; ++j) {
                if (q != j) {
                    double fx, fy;
                    calculaForca(q, j, &fx, &fy);
                    ax += fx / corpos[q].mass;
                    ay += fy / corpos[q].mass;
                }
            }
            
            calculaVelocidade(q, ax, ay);
        }

        barreira();
    }
}

void inicializaSistemaSolar(){
    //sol
    corpos[0].x = 0.0;
    corpos[0].y = 0.0;
    corpos[0].vx = 0.0;
    corpos[0].vy = 0.0;
    corpos[0].mass = 1.989e30;
    //mercurio
    corpos[1].x = 57.909e9;
    corpos[1].y = 0.0;
    corpos[1].vx = 0.0;
    corpos[1].vy = 47.36e3;
    corpos[1].mass = 0.33011e24;
    //vênus
    corpos[2].x = 108.209e9;
    corpos[2].y = 0.0;
    corpos[2].vx = 0.0;
    corpos[2].vy = 35.02e3;
    corpos[2].mass = 4.8675e24;
    //terra
    corpos[3].x = 149.596e9;
    corpos[3].y = 0.0;
    corpos[3].vx = 0.0;
    corpos[3].vy = 29.78e3;
    corpos[3].mass = 5.9724e24;
    //marte
    corpos[4].x = 227.923e9;
    corpos[4].y = 0.0;
    corpos[4].vx = 0.0;
    corpos[4].vy = 24.07e3;
    corpos[4].mass = 0.64171e24;
    //jupiter
    corpos[5].x = 778.570e9;
    corpos[5].y = 0.0;
    corpos[5].vx = 0.0;
    corpos[5].vy = 13e3;
    corpos[5].mass = 1898.19e24;
    //saturno
    corpos[6].x = 1433.529e9;
    corpos[6].y = 0.0;
    corpos[6].vx = 0.0;
    corpos[6].vy = 9.68e3;
    corpos[6].mass = 568.34e24;
    //Urano
    corpos[7].x = 2872.463e9;
    corpos[7].y = 0.0;
    corpos[7].vx = 0.0;
    corpos[7].vy = 6.80e3;
    corpos[7].mass = 86.813e24;
    //netuno
    corpos[8].x = 4495.060e9;
    corpos[8].y = 0.0;
    corpos[8].vx = 0.0;
    corpos[8].vy = 5.43e3;
    corpos[8].mass = 102.413e24;
}

void inicializaSistemaAleatório(){
    for(int i = 0; i < N_CORPOS; i++){
        corpos[i].x = rand();
        corpos[i].y = rand();
        corpos[i].vx = rand();
        corpos[i].vy = rand();
        corpos[i].mass = rand();
    }
}

int main(){
    //inicializaSistemaAleatório();
    inicializaSistemaSolar();

    pthread_t *tid; //indentificador das threads
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL){
        printf("Erro no malloc\n");
        return 2;
    }

    // Abrir arquivo
    output_file = fopen(OUTPUT_FILE, "w");
    if (output_file == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        exit(EXIT_FAILURE);
    }

    //inicializa o mutex e o cond
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);
    
    double inicio, fim, delta; //contador de tempo
    GET_TIME(inicio);

    //arguemto de cada tarefa
    Argumento arg[nthreads];

    int corposPorThread = N_CORPOS / nthreads;

    for(int i = 0; i < nthreads; i++){
        arg[i].inicio = i * corposPorThread;
        if(i < nthreads - 1){
            arg[i].fim = i * corposPorThread + corposPorThread - 1;
        }
        else{
            arg[i].fim = N_CORPOS - 1;
        }
        pthread_create(&tid[i], NULL, tarefa, (void*) (arg+i));
    }

    //junta as threads
    for(int i = 0; i < nthreads; i++){
        if(pthread_join(tid[i], NULL)){
            printf("Erro no Join\n");
            return 3;
        }
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de execucao: %lf\n", delta);

    // Fecha arquivo
    fclose(output_file);

    free(tid);
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
}
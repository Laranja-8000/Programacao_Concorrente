#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<pthread.h>
#include "timer.h"

#define G 6.67e-11  // Constante gravitacional
#define DT 86400    // Passo de tempo
#define OUTPUT_FILE "output.txt"  // Nome do arquivo de saída

typedef struct{
    int inicio;
    int fim;
} Argument;

typedef struct {
    double x, y;     // Posição
    double vx, vy;   // Velocidade
    double mass;     // Massa
} Body;

// Variáveis globais
Body bodies[9];
double duration = 86400 * 365 * 10;
int NUM_BODIES = 9;
FILE *output_file;  // Arquivo

//variáveis barreira
const int nthreads = 3;
int contador = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//barreira
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

void update_position(int index, double dt) {
    bodies[index].x += bodies[index].vx * dt;
    bodies[index].y += bodies[index].vy * dt;
}

void update_velocity(int index, double ax, double ay, double dt) {
    bodies[index].vx += ax * dt;
    bodies[index].vy += ay * dt;
}

void calculate_force(int i, int j, double *ax, double *ay) {
    double dx = bodies[j].x - bodies[i].x;
    double dy = bodies[j].y - bodies[i].y;
    double distance = sqrt(dx * dx + dy * dy);
    double force = G * bodies[i].mass * bodies[j].mass / (distance * distance);

    *ax = force * dx / distance;
    *ay = force * dy / distance;
}

void write_positions_to_file(int q) {
    fprintf(output_file, "%.2f %.2f\n", bodies[q].x, bodies[q].y);
}

pthread_mutex_t mutexFile;

void *tarefa(void *arg){
    Argument *args = (Argument*) arg;
    int inicio = args->inicio;
    int fim = args->fim;

    //printf("inicio: %d, fim: %d\n", inicio, fim);

    int q = inicio;
    int i, i2, j;

    printf("inicio: %d, fim: %d\n", inicio, fim);

    //itera sobre as partículas
    for (double t = 0; t < duration; t += DT) {
        for (i = inicio; i <= fim; i++){
            q = i;
            // Escrever posições no arquivo
            write_positions_to_file(q);

            // Atualizar posições
            update_position(q, DT);
        }
        barreira();
        for (i2 = inicio; i2 <= fim; i2++){
            q = i2;

            // Calcular forças e atualizar velocidades
            double ax = 0, ay = 0;
            for (j = 0; j < NUM_BODIES; ++j) {
                if (q != j) {
                    double fx, fy;
                    calculate_force(q, j, &fx, &fy);
                    ax += fx / bodies[q].mass;
                    ay += fy / bodies[q].mass;
                }
            }
            update_velocity(q, ax, ay, DT);
        }
    }
    printf("fim da thread %d\n", inicio);
}

void inicializarCorpos(){
    //sol
    bodies[0].x = 0.0;
    bodies[0].y = 0.0;
    bodies[0].vx = 0.0;
    bodies[0].vy = 0.0;
    bodies[0].mass = 1.989e30;
    //mercurio
    bodies[1].x = 57.909e9;
    bodies[1].y = 0.0;
    bodies[1].vx = 0.0;
    bodies[1].vy = 47.36e3;
    bodies[1].mass = 0.33011e24;
    //vênus
    bodies[2].x = 108.209e9;
    bodies[2].y = 0.0;
    bodies[2].vx = 0.0;
    bodies[2].vy = 35.02e3;
    bodies[2].mass = 4.8675e24;
    //terra
    bodies[3].x = 149.596e9;
    bodies[3].y = 0.0;
    bodies[3].vx = 0.0;
    bodies[3].vy = 29.78e3;
    bodies[3].mass = 5.9724e24;
    //marte
    bodies[4].x = 227.923e9;
    bodies[4].y = 0.0;
    bodies[4].vx = 0.0;
    bodies[4].vy = 24.07e3;
    bodies[4].mass = 0.64171e24;
    //jupiter
    bodies[5].x = 778.570e9;
    bodies[5].y = 0.0;
    bodies[5].vx = 0.0;
    bodies[5].vy = 13e3;
    bodies[5].mass = 1898.19e24;
    //saturno
    bodies[6].x = 1433.529e9;
    bodies[6].y = 0.0;
    bodies[6].vx = 0.0;
    bodies[6].vy = 9.68e3;
    bodies[6].mass = 568.34e24;
    //Urano
    bodies[7].x = 2872.463e9;
    bodies[7].y = 0.0;
    bodies[7].vx = 0.0;
    bodies[7].vy = 6.80e3;
    bodies[7].mass = 86.813e24;
    //netuno
    bodies[8].x = 4495.060e9;
    bodies[8].y = 0.0;
    bodies[8].vx = 0.0;
    bodies[8].vy = 5.43e3;
    bodies[8].mass = 102.413e24;
}

int main(){
    inicializarCorpos();

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

    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);
    
    double inicio, fim, delta; //contador de tempo
    GET_TIME(inicio);

    //arguemto de cada tarefa
    Argument arg[nthreads];

    int corposThread = NUM_BODIES / nthreads;

    for(int i = 0; i < nthreads; i++){
        arg[i].inicio = i * corposThread;
        if(i < nthreads - 1){
            arg[i].fim = i * corposThread + corposThread - 1;
        }
        else{
            arg[i].fim = NUM_BODIES - 1;
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
    printf("Tempo de execucao: %lf", delta);

    // Fechar arquivo
    fclose(output_file);

    free(tid);

    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}
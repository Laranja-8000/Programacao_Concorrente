#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define G 1  // Constante gravitacional
#define DT 0.1         // Passo de tempo
#define OUTPUT_FILE "output.txt"  // Nome do arquivo de saída

typedef struct {
    double x, y;     // Posição
    double vx, vy;   // Velocidade
    double mass;     // Massa
} Body;

// Variáveis globais
Body bodies[2];
double duration = 20.0;
int NUM_BODIES = 2;
FILE *output_file;  // Agora é uma variável global

//variáveis barreira
const int nthreads = 2;
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
    long int q = (long int) arg;

    int i, j;
    for (double t = 0; t < duration; t += DT) {
        printf("thread %d\n", q);

        // Escrever posições no arquivo
        write_positions_to_file(q);

        // Atualizar posições
        update_position(q, DT);

        barreira();

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

        barreira();
    }
}

int main() {
    // Inicializar corpos
    bodies[0].x = 0.0;
    bodies[0].y = 0.0;
    bodies[0].vx = 0.0;
    bodies[0].vy = 0.0;
    bodies[0].mass = 1.0;

    bodies[1].x = 1.0;
    bodies[1].y = 0.0;
    bodies[1].vx = 0.0;
    bodies[1].vy = 1.0;
    bodies[1].mass = 1.0;
    
    pthread_t *tid; //identificador das threads

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

    for(int i = 0; i < nthreads; i++){
        pthread_create(&tid[i], NULL, tarefa, (void*) i);
    }

    //junta as threads
    for(int i = 0; i < nthreads; i++){
        if(pthread_join(tid[i], NULL)){
            printf("Erro no Join\n");
            return 3;
        }
    }

    // Fechar arquivo
    fclose(output_file);

    free(tid);

    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G 1  // Constante gravitacional
#define DT 1.0       // Passo de tempo
#define OUTPUT_FILE "output.txt"  // Nome do arquivo de saída

typedef struct {
    double x, y;     // Posição
    double vx, vy;   // Velocidade
    double mass;     // Massa
} Body;

void update_position(Body *body, double dt) {
    body->x += body->vx * dt;
    body->y += body->vy * dt;
}

void update_velocity(Body *body, double ax, double ay, double dt) {
    body->vx += ax * dt;
    body->vy += ay * dt;
}

void calculate_force(Body *a, Body *b, double *ax, double *ay) {
    double dx = b->x - a->x;
    double dy = b->y - a->y;
    double distance = sqrt(dx*dx + dy*dy);
    double force = G * a->mass * b->mass / (distance * distance);

    *ax = force * dx / distance;
    *ay = force * dy / distance;
}

void write_positions_to_file(Body *bodies, int num_bodies, FILE *file) {
    for (int i = 0; i < num_bodies; ++i) {
        fprintf(file, "%.2f %.2f\n", bodies[i].x, bodies[i].y);
    }
}

void simulate_n_body(Body *bodies, int num_bodies, double duration) {
    FILE *output_file = fopen(OUTPUT_FILE, "w");
    if (output_file == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        exit(EXIT_FAILURE);
    }

    int i, j;
    for (double t = 0; t < duration; t += DT) {
        // Escrever posições no arquivo
        write_positions_to_file(bodies, num_bodies, output_file);

        // Atualizar posições
        for (i = 0; i < num_bodies; ++i) {
            update_position(&bodies[i], DT);
        }

        // Calcular forças e atualizar velocidades
        for (i = 0; i < num_bodies; ++i) {
            double ax = 0, ay = 0;
            for (j = 0; j < num_bodies; ++j) {
                if (i != j) {
                    double fx, fy;
                    calculate_force(&bodies[i], &bodies[j], &fx, &fy);
                    ax += fx / bodies[i].mass;
                    ay += fy / bodies[i].mass;
                }
            }
            update_velocity(&bodies[i], ax, ay, DT);
        }
    }

    fclose(output_file);
}

#define NUM_BODIES 2
int main() {
    // Definir parâmetros da simulação
    //int num_bodies = 2;
    double duration = 20.0;

    // Inicializar corpos
    Body bodies[NUM_BODIES];
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

    // Simular N-corpo e salvar posições no arquivo
    simulate_n_body(bodies, NUM_BODIES, duration);

    return 0;
}

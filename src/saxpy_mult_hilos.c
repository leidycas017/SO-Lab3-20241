#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <stdatomic.h>

struct datos_hilo
{
    double *X;
    double *Y;
    double *Y_avgs;
    double a;
    int p;
    int max_iters;
    int id_hilo;
    int n_threads;
};

void parseo_argumentos(int argc, char *argv[], unsigned int *seed, int *p, int *n_threads, int *max_iters);
void inicializar_datos(double **X, double **Y, double **Y_avgs, int p, int max_iters, unsigned int seed);
void *saxpy_iteration(void *arg);
double calcular_tiempo(struct timeval t_inicio, struct timeval t_final);
void imprimir_resultados(double *Y, double *Y_avgs, int p, int max_iters);
void mostrarPresentacion();
void limpiarPantalla();
void parseo_argumentos(int argc, char* argv[], unsigned int* seed, int* p, int* n_threads, int* max_iters) {
    int opt;
    while((opt = getopt(argc, argv, ":p:s:n:i:")) != -1){  
        switch(opt){  
            case 'p':  
                printf("tamaño del vector: %s\n", optarg);
                *p = strtol(optarg, NULL, 10);
                assert(*p > 0 && *p <= 2147483647);
                break;  
            case 's':  
                printf("semilla: %s\n", optarg);
                *seed = strtol(optarg, NULL, 10);
                break;
            case 'n':  
                printf("número de hilos: %s\n", optarg);
                *n_threads = strtol(optarg, NULL, 10);
                break;  
            case 'i':  
                printf("máx. iteraciones: %s\n", optarg);
                *max_iters = strtol(optarg, NULL, 10);
                break;  
            case ':':  
                printf("la opción -%c necesita un valor\n", optopt);  
                break;  
            case '?':  
                fprintf(stderr, "Uso: %s [-p <tamaño del vector>] [-s <semilla>] [-n <número de hilos>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }  
    }
}

void inicializar_datos(double** X, double** Y, double** Y_avgs, int p, int num_iter, unsigned int seed) {
    int i;
    *X = (double*) malloc(sizeof(double) * p);
    *Y = (double*) malloc(sizeof(double) * p);
    *Y_avgs = (double*) malloc(sizeof(double) * num_iter);

    srand(seed);

    for(i = 0; i < p; i++){
        (*X)[i] = (double)rand() / RAND_MAX;
        (*Y)[i] = (double)rand() / RAND_MAX;
    }
    for(i = 0; i < num_iter; i++){
        (*Y_avgs)[i] = 0.0;
    }
}

void* saxpy_iteration(void* arg) {
    struct datos_hilo* datos = (struct datos_hilo*)arg;
    int tamano = datos->p / datos->n_threads;
    int indice_inicio = datos->id_hilo * tamano;
    int indice_final = (datos->id_hilo == datos->n_threads - 1) ? datos->p - 1 : indice_inicio + tamano - 1; 
    for (int it = 0; it < datos->max_iters; it++) {
        double sum = 0.0;
        for (int i = indice_inicio; i <= indice_final; i++) {
            datos->Y[i] = datos->Y[i] + datos->a * datos->X[i];
            sum += datos->Y[i];  
        }
        // Suma atómica
        datos->Y_avgs[it] = datos->Y_avgs[it] + sum;
       // atomic_exchange(&datos->Y_avgs[it], atomic_load(&datos->Y_avgs[it]) + sum);
        // Divide la suma por el tamaño del bloque
        datos->Y_avgs[it] = datos->Y_avgs[it] / (double)(indice_final - indice_inicio + 1);
       // atomic_exchange(&datos->Y_avgs[it], atomic_load(&datos->Y_avgs[it]) / (double)(indice_final - indice_inicio + 1));
    }

    pthread_exit(NULL);
}

double calcular_tiempo(struct timeval t_inicio, struct timeval t_final) {
    double tiempo = (t_final.tv_sec - t_inicio.tv_sec) * 1000.0;  
    tiempo += (t_final.tv_usec - t_inicio.tv_usec) / 1000.0;
    return tiempo;
}

void imprimir_resultados(double* Y, double* Y_avgs, int p, int max_iters) {
    printf("Últimos 3 valores de Y: %f, %f, %f \n", Y[p-3], Y[p-2], Y[p-1]);
    printf("Últimos 3 valores de Y_avgs: %f, %f, %f \n", Y_avgs[max_iters-3], Y_avgs[max_iters-2], Y_avgs[max_iters-1]);
}

void mostrarPresentacion() {
    limpiarPantalla(); 
    printf("\n\n");
    printf("*****************************************************************\n");
    printf("*             Bienvenido al Programa Saxpy                      *\n");
    printf("*                                                               *\n");
    printf("*             TRABAJO HECHO POR Yuly Alvear Romo                *\n");
    printf("*             TRABAJO HECHO POR Leidy Castaño                   *\n");
    printf("*             TRABAJO HECHO POR Omar alberto Torres             *\n");
    printf("*                                                               *\n");
    printf("*****************************************************************\n");
    printf("\n");
    printf("Iniciando programa...\n");
    printf("\n");
    printf("Cargando ");
    for (int i = 0; i < 10; i++) {
        printf(".");
        fflush(stdout); 
        usleep(300000); 
    }
    printf("\n\n");
}

void limpiarPantalla() {
    printf("\033[2J\033[H");  
}

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

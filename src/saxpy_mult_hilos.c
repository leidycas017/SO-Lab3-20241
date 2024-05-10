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

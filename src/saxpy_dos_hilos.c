/**
 * @defgroup   SAXPY saxpy
 *
 * @brief      This file implements an iterative saxpy operation
 * 
 * @param[in] <-p> {vector size} 
 * @param[in] <-s> {seed}
 * @param[in] <-n> {number of threads to create} 
 * @param[in] <-i> {maximum iterations} 
 *
 * @author     Danny Munera
 * @date       2020
 * @estudiantes Yuly Alvear Romo, Leidy Castaño, Omar Torres
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

struct datos_hilo {
    pthread_mutex_t mutex;
    double* X;
    double* Y;
    double* Y_avgs;
    double a;
    int p;
    int max_iters;
    int id_hilo;
    int n_threads; 
};

void parseo_argumentos(int argc, char* argv[], unsigned int* seed, int* p, int* n_threads, int* max_iters);
void inicializar_datos(double** X, double** Y, double** Y_avgs, int p, int max_iters, unsigned int seed);
double calcular_tiempo(struct timeval t_inicio, struct timeval t_final);
void imprimir_resultados(double* Y, double* Y_avgs, int p, int max_iters);
void* hilo_saxpy(void* arg);
void mostrarPresentacion();
void limpiarPantalla();

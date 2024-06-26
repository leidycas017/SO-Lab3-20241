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
#include <stdatomic.h>

struct datos_hilo
{
    pthread_mutex_t mutex;
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
double calcular_tiempo(struct timeval t_inicio, struct timeval t_final);
void imprimir_resultados(double *Y, double *Y_avgs, int p, int max_iters);
void *hilo_saxpy(void *arg);
void mostrarPresentacion();
void limpiarPantalla();
/**
 * Función que parsea los parametros que ingresan por línea de comandos
 */
void parseo_argumentos(int argc, char *argv[], unsigned int *seed, int *p, int *n_threads, int *max_iters)
{
    int opt;
    while ((opt = getopt(argc, argv, ":p:s:n:i:")) != -1)
    {
        switch (opt)
        {
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
            //*n_threads = strtol(optarg, NULL, 10);
            *n_threads = 2;
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
/**
 * Funcion que inicializa para asignar memoria dinámica a los arreglos y inicializar con valores seudo aleatorios
 */
void inicializar_datos(double **X, double **Y, double **Y_avgs, int p, int num_iter, unsigned int seed)
{
    int i;
    *X = (double *)malloc(sizeof(double) * p);
    *Y = (double *)malloc(sizeof(double) * p);
    *Y_avgs = (double *)malloc(sizeof(double) * num_iter);

    srand(seed);

    for (i = 0; i < p; i++)
    {
        (*X)[i] = (double)rand() / RAND_MAX;
        (*Y)[i] = (double)rand() / RAND_MAX;
    }
    for (i = 0; i < num_iter; i++)
    {
        (*Y_avgs)[i] = 0.0;
    }
}

/**
 *  Funcion que calcula el tiempo que demoraron los hilos en ejecucion
 */
double calcular_tiempo(struct timeval t_inicio, struct timeval t_final)
{
    double tiempo = (t_final.tv_sec - t_inicio.tv_sec) * 1000.0; // segundos a milisegundos
    tiempo += (t_final.tv_usec - t_inicio.tv_usec) / 1000.0;     // microsegundos a milisegundos
    return tiempo;
}
/**
 * Función para imprimir resultados
 */
void imprimir_resultados(double *Y, double *Y_avgs, int p, int max_iters)
{
    printf("Últimos 3 valores de Y: %f, %f, %f \n", Y[p - 3], Y[p - 2], Y[p - 1]);
    printf("Últimos 3 valores de Y_avgs: %f, %f, %f \n", Y_avgs[max_iters - 3], Y_avgs[max_iters - 2], Y_avgs[max_iters - 1]);
}
/**
 * La funcion es esponsabel de dividir el arreglo en dos parte parte 1 en el rango (0, 499) y parte 2 en el rango (500, 999)
 * Se realizan los calculos para cada parte Y = Y + a*X y se obtiene un promedio
 */
void *hilo_saxpy(void *arg)
{
    struct datos_hilo *datos = (struct datos_hilo *)arg;
    int tamano = datos->p / 2;
    int indice_inicio = datos->id_hilo * tamano;
    int indice_final = (datos->id_hilo == datos->n_threads - 1) ? datos->p - 1 : indice_inicio + (tamano - 1);
    for (int it = 0; it < datos->max_iters; it++)
    {
        double sum = 0.0;
        for (int i = indice_inicio; i <= indice_final; i++)
        {
            datos->Y[i] = datos->Y[i] + datos->a * datos->X[i];
            sum += datos->Y[i];
        }
        // Realiza la suma atómica
        atomic_exchange(&datos->Y_avgs[it], sum);
        // Divide la suma por el tamaño del bloque
        atomic_exchange(&datos->Y_avgs[it], atomic_load(&datos->Y_avgs[it]) / tamano);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    limpiarPantalla();     // Limpiar la pantalla antes de iniciar el programa
    mostrarPresentacion(); // Mostrar una presentación del programa
    unsigned int seed;
    int p;
    int n_threads;
    int max_iters;

    // Analizar los argumentos de línea de comandos para obtener los parámetros
    parseo_argumentos(argc, argv, &seed, &p, &n_threads, &max_iters);

    // Imprimir los parámetros obtenidos
    printf("p = %d, seed = %d, n_threads = %d, max_iters = %d\n", p, seed, n_threads, max_iters);

    // Inicializar los vectores X, Y, y los promedios de Y con valores pseudoaleatorios
    double *X;
    double *Y;
    double *Y_avgs;
    inicializar_datos(&X, &Y, &Y_avgs, p, max_iters, seed);

    // Generar un valor aleatorio para 'a'
    double a = (double)rand() / RAND_MAX;

    // Variables para medir el tiempo de ejecución
    struct timeval t_inicio, t_final;

    // Identificadores de los hilos
    pthread_t hilo1, hilo2;

    // Estructuras de datos para los hilos
    struct datos_hilo datos_hilo1, datos_hilo2;

    // Configurar datos para el primer hilo
    datos_hilo1.X = X;
    datos_hilo1.Y = Y;
    datos_hilo1.Y_avgs = Y_avgs;
    datos_hilo1.a = a;
    datos_hilo1.p = p;
    datos_hilo1.max_iters = max_iters;
    datos_hilo1.id_hilo = 0;

    // Configurar datos para el segundo hilo
    datos_hilo2.X = X;
    datos_hilo2.Y = Y;
    datos_hilo2.Y_avgs = Y_avgs;
    datos_hilo2.a = a;
    datos_hilo2.p = p;
    datos_hilo2.max_iters = max_iters;
    datos_hilo2.id_hilo = 1;

    // Obtener el tiempo de inicio
    gettimeofday(&t_inicio, NULL);

    // Crear los hilos
    pthread_create(&hilo1, NULL, hilo_saxpy, (void *)&datos_hilo1);
    pthread_create(&hilo2, NULL, hilo_saxpy, (void *)&datos_hilo2);

    // Esperar a que los hilos terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    // Obtener el tiempo de finalización
    gettimeofday(&t_final, NULL);

    // Calcular e imprimir el tiempo de ejecución
    double tiempo_ejecucion = calcular_tiempo(t_inicio, t_final);
    printf("Tiempo de ejecución: %f ms \n", tiempo_ejecucion);

    // Imprimir los resultados finales
    imprimir_resultados(Y, Y_avgs, p, max_iters);

    // Liberar la memoria asignada
    free(X);
    free(Y);
    free(Y_avgs);

    return 0;
}

void mostrarPresentacion()
{
    limpiarPantalla(); // Limpiar la pantalla antes de mostrar la presentación
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
    for (int i = 0; i < 10; i++)
    {
        printf(".");
        fflush(stdout); // Limpiar el buffer de salida para mostrar los puntos
        usleep(300000); // Esperar 300 milisegundos entre cada punto
    }
    printf("\n\n");
}

void limpiarPantalla()
{
    system("clear");
}
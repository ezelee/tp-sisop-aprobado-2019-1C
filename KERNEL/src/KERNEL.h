/*
 * KERNEL.h
 *
 *  Created on: 13 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <shared.h>
#include <gossiping.h>
#include "parser.h"
#include "Auxiliares/Logueo.h"
#include "Auxiliares/Consola.h"
#include "Auxiliares/Metricas.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Hilos.h"
#include "Auxiliares/Criterios.h"
#include <semaphore.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define MAX_PATH 50

/* Variables globales */
pthread_t thread_consola;
pthread_t thread_gossiping;
pthread_t inotify;
pthread_t thread_refresh;
pthread_t thread_planificador;
pthread_t thread_metricas;
pthread_mutex_t mutex_metadata;
pthread_mutex_t mutex_memoria_sc;
pthread_mutex_t mutex_memoria_shc;
pthread_mutex_t mutex_memoria_ev;
pthread_mutex_t mutex_asociadas;

pthread_mutex_t mutex_LISTA_CONN;

// Listas planificacion
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_exit;
//pthread_mutex_t mutex_request;


/* Definición de tipos */
sem_t sem_new, sem_ready, sem_exec, sem_exit, sem_multiprog;

//int memoria_sc;
t_tipoSeeds* memoria_sc;
t_list *lista_criterio_shc;
t_list *lista_criterio_ev;
t_list *lista_metadata;
t_list *LISTA_CONN;
t_list *mem_asociadas;
char** lista_ips;
char** lista_puertos;


/* Declaración de Procesos*/
void inicializar();
void inicializar_semaforos();
void inicializar_listas();

#endif /* KERNEL_H_ */

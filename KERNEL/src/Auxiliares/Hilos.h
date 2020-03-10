/*
 * Hilos.h
 *
 *  Created on: 17 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_HILOS_H_
#define AUXILIARES_HILOS_H_

#include "../KERNEL.h"
#include <stdlib.h>
#include "Planificador.h"

// Funciones
void *crearInotify();
void *hiloGossiping();
void init_gossiping();
void *inicializar_refresh();
int hay_memorias_disponibles();
t_tipoSeeds* get_memoria_conectada();

// Creación de Hilos
void crear_hilo_consola();
int crear_hilo_inotify();
void crear_hilo_gossiping();
void crear_hilo_refresh();
void crear_hilo_planificador();
void crear_hilo_metricas();

#endif /* AUXILIARES_HILOS_H_ */

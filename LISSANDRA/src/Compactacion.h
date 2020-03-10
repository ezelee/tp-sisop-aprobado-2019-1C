/*
 * Compactacion.h
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */

#ifndef COMPACTACION_H_
#define COMPACTACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <shared.h>
#include <parser.h>
#include <string.h>
#include <signal.h>

typedef struct {
	int retardo;
	char path_tabla[50];
}t_datos_compactacion;

typedef struct {
	pthread_t hilo;
	char path_tabla[50];
}t_datos_hilo_compactacion;

t_list* listaHilos;
pthread_t compactacion;
int hilo_compactacion;

int crearHiloCompactacion();
int process_compactaction(char *nombreTabla);

#endif /* COMPACTACION_H_ */

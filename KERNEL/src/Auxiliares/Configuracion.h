/*
 * Configuracion.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <shared.h>
#include "Logueo.h"

// Definición de tipos
typedef struct {
	char* ip;
	char* puerto;
	char** ip_memoria;
	char** puerto_memoria;
	int quantum;
	int multiprocesamiento;
	int metadata_refresh;
	int sleep_ejecucion;
	int retardo_gossiping;
} t_kernel_conf;


// Variables globales
t_config *config;
t_kernel_conf kernel_conf;


void cargar_configuracion_kernel();
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */

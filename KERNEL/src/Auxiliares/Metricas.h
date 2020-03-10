/*
 * Metricas.h
 *
 *  Created on: 25 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_METRICAS_H_
#define AUXILIARES_METRICAS_H_

#include "../KERNEL.h"

// Structs
typedef struct {
	float duracion;
	int contador;
} t_metrics;

t_metrics metric_insert;
t_metrics metric_select;


void* metricas();
void get_metricas_log();
void get_metricas_consola();
void resetear_metricas();
float get_read_latency();
float get_write_latency();




#endif /* AUXILIARES_METRICAS_H_ */

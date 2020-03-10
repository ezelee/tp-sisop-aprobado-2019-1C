/*
 * Metricas.c
 *
 *  Created on: 25 jul. 2019
 *      Author: utnso
 */

#include "Metricas.h"

void* metricas() {

	resetear_metricas();
	while (1) {
		sleep(30);
		get_metricas_log();
		resetear_metricas();
	}
}
/*Read Latency / 30s: El tiempo promedio que tarda un SELECT en ejecutarse en los últimos 30 segundos.
 Write Latency / 30s: El tiempo promedio que tarda un INSERT en ejecutarse en los últimos 30 segundos.
 Reads / 30s: Cantidad de SELECT ejecutados en los últimos 30 segundos.
 Writes / 30s: Cantidad de INSERT ejecutados en los últimos 30 segundos.
 Memory Load (por cada memoria):  Cantidad de INSERT / SELECT que se ejecutaron en esa memoria respecto de las operaciones totales.
 */
void get_metricas_log() {
	log_info(logger, "METRICS| ---------- Logueando métricas ---------- ");
	float tiempoSelect = get_read_latency();
	float tiempoInsert = get_write_latency();
	log_info(logger, "METRICS| Read Latency:%f", tiempoSelect);
	log_info(logger, "METRICS| Write Latency:%f", tiempoInsert);
	log_info(logger, "METRICS| Reads:%d", metric_select.contador);
	log_info(logger, "METRICS| Writes:%d", metric_insert.contador);
}

void resetear_metricas() {
	metric_insert.contador = 0;
	metric_insert.duracion = 0.00;
	metric_select.contador = 0;
	metric_select.duracion = 0.00;
}

void get_metricas_consola() {
	float tiempoSelect = get_read_latency();
	float tiempoInsert = get_write_latency();
	printf("METRICS| Read Latency: %.2f segundos\n", tiempoSelect);
	printf("METRICS| Write Latency: %.2f segundos\n", tiempoInsert);
	printf("METRICS| Reads:%d\n", metric_select.contador);
	printf("METRICS| Writes:%d\n", metric_insert.contador);
}

float get_read_latency() {
	if(metric_select.contador == 0) return 0;

	float tiempoPromedio = (metric_select.contador / metric_select.duracion);

	return tiempoPromedio;
}

float get_write_latency() {
	if(metric_insert.contador == 0) return 0;

	float tiempoPromedio = (metric_insert.contador / metric_insert.duracion);

	return tiempoPromedio;
}

/*
 * Hilos.c
 *
 *  Created on: 17 jul. 2019
 *      Author: utnso
 */

#include "Hilos.h"
#include "Metadata.h"
#include "Metricas.h"

// Funciones
void *crearInotify() {
	int inotifyFd = inotifyInit("../config/KERNEL.config");

	while (1) {/* Read events forever */
		if (inotifyEvent(inotifyFd))
			cargar_configuracion_kernel_inotify("../config/KERNEL.config");
	}
}

void *hiloGossiping() {
	int tiempo = 15000;
	sleep(tiempo / 1000);

	while (1) {
		processGossipingStruct(logger, LISTA_CONN, kernelGoss);
		if (hay_memorias_disponibles()) {
			log_info(logger, "GOSSIP| Memorias conectadas!");
		}

		sleep(tiempo / 1000);
	}
}

void init_gossiping() {

	crearListaSeedsStruct(kernelGoss, kernel_conf.ip, kernel_conf.puerto, 1000,
			kernel_conf.ip_memoria, kernel_conf.puerto_memoria, logger,
			LISTA_CONN);
}

void *inicializar_refresh() {

	while (1) {
		aplicar_tiempo_refresh();
//		pthread_mutex_lock(&mutex_metadata);
		if (hay_memorias_disponibles()) {
			log_info(logger, "REFRESH| Hay memoria disponible");
			actualizar_metadata();
		} else {
			log_info(logger, "REFRESH| No Hay memoria disponible");
		}
//		pthread_mutex_unlock(&mutex_metadata);
	}

}

int hay_memorias_disponibles() {

	int memoria_conectada = 0;
	pthread_mutex_lock(&mutex_LISTA_CONN);
	for (int i = 0; i < LISTA_CONN->elements_count; i++) {
		t_tipoSeeds* mem = list_get(LISTA_CONN, i);

		if (mem->estado == 1) {
			memoria_conectada = 1;
			break;
		}
	}

	int result = LISTA_CONN->elements_count > 0 && memoria_conectada > 0;
	pthread_mutex_unlock(&mutex_LISTA_CONN);

	return result;
}

t_tipoSeeds* get_memoria_conectada() {

	int i;

	pthread_mutex_lock(&mutex_LISTA_CONN);
	for (i = 0; i < LISTA_CONN->elements_count; i++) {
		t_tipoSeeds* mem = list_get(LISTA_CONN, i);

		if (mem->estado == 1) {
			break;
		}
	}

	t_tipoSeeds* seed =  list_get(LISTA_CONN, i);
	pthread_mutex_unlock(&mutex_LISTA_CONN);

	return seed;
}

// Creación de Hilos
void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola,
			NULL);
	if (hilo_consola == -1) {
		log_info(logger, "THREAD| Error al generar hilo consola.");
	}
	log_info(logger, "THREAD| Hilo consola OK.");
}

int crear_hilo_inotify() {
	sigset_t set;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	hilo_inotify = pthread_create(&inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
		log_error(logger,
				"THREAD| Error al generar hilo I-NOTIFY.");
	}
	log_info(logger, "THREAD| Hilo I-NOTIFY OK.");

	return 1;
}

void crear_hilo_gossiping() {

	int hilo_gossiping = pthread_create(&thread_gossiping, NULL, hiloGossiping,
			NULL);
	if (hilo_gossiping == -1) {
		log_error(logger,
				"THREAD| Error al generar hilo Gossiping.");
	}
	log_info(logger, "THREAD| Hilo Gossiping OK.");
}

void crear_hilo_refresh() {

	int hiloDump = pthread_create(&thread_refresh, NULL, inicializar_refresh,
	NULL);
	if (hiloDump == -1) {
		log_error(logger, "THREAD| Error al generar hilo Refresh.");
	}
	log_info(logger, "THREAD| Hilo Refresh OK.");
}

void crear_hilo_planificador() {

	int hilo_planificador = pthread_create(&thread_planificador, NULL,
			planificar, NULL);
	if (hilo_planificador == -1) {
		log_error(logger,
				"THREAD| Error al generar hilo Planificador.");
	}
	log_info(logger, "THREAD| Hilo Planificador OK.");
}

void crear_hilo_metricas() {

	int hilo_metricas = pthread_create(&thread_metricas, NULL,
			metricas, NULL);
	if (hilo_metricas == -1) {
		log_error(logger,
				"THREAD| Error al generar hilo Metricas.");
	}
	log_info(logger, "THREAD| Hilo Metricas OK.");
}

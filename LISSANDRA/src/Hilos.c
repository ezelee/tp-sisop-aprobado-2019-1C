/*
 * InotifyLissandra.c
 *
 *  Created on: 14 jul. 2019
 *      Author: utnso
 */
#include "Hilos.h"

void CrearHiloConsola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola");
	}
	log_info(logger, "Se generó el hilo para la consola");
}

void CrearHiloConexiones() {

	int hilo_conexiones = pthread_create(&thread_conexiones, NULL, listen_connexions, NULL);
		if (hilo_conexiones == -1) {
			log_error(logger, "No se pudo generar el hilo para las conexiones");
		}
		log_info(logger, "Se generó el hilo para las conexiones");
}

void CrearHiloDump() {

	int hiloDump = pthread_create(&thread_dump, NULL, InicializarDump, NULL);
	if (hiloDump == -1) {
		log_error(logger, "No se pudo generar el hilo para proceso dump");
	}
	log_info(logger, "Se generó el hilo para el dump");
}

void *crearInotify() {
	int inotifyFd = inotifyInit("../config/Lissandra.config");

	if (inotifyFd == -1)
		_exit_with_error("inotify_init", NULL);

	while (1) {/* Read events forever */
		if (inotifyEvent(inotifyFd))
			get_modify_config("../config/Lissandra.config");
	}
}

int CrearHiloInotify() {
	sigset_t set;
	int s;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",
		NULL);

	hilo_inotify = pthread_create(&thread_inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
		log_error(logger, "No se pudo generar el hilo para el I-NOTIFY.");
	}
	log_info(logger, "Se generó el hilo para el I-NOTIFY.");

	return 1;
}

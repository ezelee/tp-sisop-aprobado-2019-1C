/*
 * gossipingMemoria.c
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */
#include <shared.h>

#include "MEM.h"

#include "connection.h"
#include "file_conf.h"
#include "error.h"
#include "memory.h"

#include "gossipingMemoria.h"
#include "gossiping.h"

void *hiloGossipingMemoria()
{
	sleep (MEM_CONF.RETARDO_GOSSIPING/1000);

	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"INIT_HILO_GOSSIPING");
		processGossipingStruct(logger,LISTA_CONN,gossiping);
		loggear(logger,LOG_LEVEL_INFO,"END_HILO_GOSSIPING");

		sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
	}
}

int crearHiloGossipingMemoria()
{
	sigset_t set;
	int s;
	int hilo_cliente;
	LISTA_CONECTADOS =list_create();

	pthread_mutex_init(&mutexGossiping, NULL);
	pthread_mutex_init(&mutexprocessGossiping, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		return -1;
		//_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_cliente = pthread_create(&cliente, NULL, hiloGossipingMemoria, (void *) &set);

	if (hilo_cliente == -1)
		loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_GOSSIPING: %s", hilo_cliente);
	log_info(logger, "Se generó el hilo para el GOSSIPING.");

	return 1;
}

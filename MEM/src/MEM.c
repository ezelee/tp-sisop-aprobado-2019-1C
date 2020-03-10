/*
 ============================================================================
 Name        : FM9.c
 Author      : Emiliano Ibarrola
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "MEM.h"

#include <shared.h>

#include "connection.h"
#include "file_conf.h"
#include "consola.h"
#include "error.h"
#include "argparse.h"
#include "memory.h"
#include "journaling.h"
#include "gossiping.h"
#include "gossipingMemoria.h"
#include "inotifyMemoria.h"

char* intToChar4(int num){
	//RECORDAR: liberar el puntero con free()
	char* n_char = string_new();//malloc(sizeof(int));
	memcpy(n_char, &num, sizeof(int));
	return n_char;
}

int main(int argc, char *argv[]) {

	LISTA_CONN = list_create();
	LISTA_CONN_PORT = list_create();
	initArgumentos(argc, argv);
	logger = configurar_logger_verbose("MEM.log", "MEM", string_equals_ignore_case(args_verbose,"true")?true:false);
	get_config(string_equals_ignore_case(args_configfile,"false")?"../MEM.conf":args_configfile);

	//TODO: Inicializar la lista de SEEDS - USAR EN KERNEL
	crearListaSeedsStruct(gossiping,MEM_CONF.IP,MEM_CONF.PUERTO,MEM_CONF.MEMORY_NUMBER,MEM_CONF.IP_SEEDS,MEM_CONF.PUERTO_SEEDS,logger,LISTA_CONN);

	tamanio_value = VALUE;
	socket_lis = connect_to_server(MEM_CONF.IP_FS, MEM_CONF.PUERTO_FS, lis,memoria);

	init_memory_spa();

	crearHiloConsola();
	crearHiloJournaling();
	crearHiloInotify();
	//TODO: Crea el hilo encargado del Gossiping - USAR EN KERNEL
	crearHiloGossipingMemoria();

	listen_connexions(MEM_CONF.PUERTO);

	exit_gracefully(EXIT_SUCCESS);
}

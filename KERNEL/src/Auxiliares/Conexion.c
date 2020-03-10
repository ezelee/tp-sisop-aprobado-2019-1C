/*
 * Conexion.c
 *
 *  Created on: 22 abr. 2019
 *      Author: utnso
 */

#include "Conexion.h"

int conectar_a_servidor(char* ip, int puerto, int proceso) {

	int socket;

	if((socket = definirSocket(logger))<= 0)
	{
		log_error(logger, "No se pudo definir socket.");
		return -1;
	}

	// sacar esto
	log_info(logger, "Socket creado: %d", socket);
	log_info(logger, "ip: %s", ip);
	log_info(logger, "puerto: %d", puerto);

	if(conectarseAServidor(socket, ip, puerto, logger)<=0)
	{
		log_error(logger, "No se pudo conectar a servidor.");
		return -1;
	}

	loggear(logger,LOG_LEVEL_INFO, "INICIO Handshake(%d)...", proceso);
	enviarMensaje(kernel, handshake, 0, NULL, socket, logger, proceso);
	t_mensaje* msg = recibirMensaje(socket, logger);
	destruirMensaje(msg);
	loggear(logger,LOG_LEVEL_INFO, "FIN Handshake(%d)", proceso);
	return socket;
}

void enviar_journal_memorias() {
	enviar_journal_sc();
	enviar_journal_shc();
	enviar_journal_ev();
}

void enviar_journal_sc() {
	if(memoria_sc == NULL) return;
//	t_tipoSeeds *memoria;
//	memoria = obtener_memoria_lista(memoria_sc->numeroMemoria);
	if (memoria_sc != NULL) {
		//TODO: MUTEX
		pthread_mutex_lock(&mutex_memoria_sc);
		enviar_mensaje_journal(memoria_sc);
		pthread_mutex_unlock(&mutex_memoria_sc);
	} else {
		log_info(logger, "Memoria criterio SC no encontrada");
	}

}

void enviar_journal_shc() {
	pthread_mutex_lock(&mutex_memoria_shc);
	int size_shc = list_size(lista_criterio_shc);
//	pthread_mutex_unlock(&mutex_memoria_shc);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_shc; i++) {
//		pthread_mutex_lock(&mutex_memoria_shc);
		memoria = list_get(lista_criterio_shc, i);
//		pthread_mutex_unlock(&mutex_memoria_shc);
		enviar_mensaje_journal(memoria);
	}
	pthread_mutex_unlock(&mutex_memoria_shc);
}

void enviar_journal_ev() {
	pthread_mutex_lock(&mutex_memoria_ev);
	int size_ev = list_size(lista_criterio_ev);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_ev; i++) {
		memoria = list_get(lista_criterio_ev, i);
		enviar_mensaje_journal(memoria);

	}
	pthread_mutex_unlock(&mutex_memoria_ev);
}

void enviar_mensaje_journal(t_tipoSeeds *memoria) {
	int puerto = atoi(memoria->puerto);
	int client_socket = conectar_a_servidor(memoria->ip, puerto, kernel);

	if (socket > 0) {
		enviarMensaje(kernel, journal, 0, NULL, client_socket, logger, mem);
		//TODO: RECIBIR MSJ
		t_mensaje* mensaje = recibirMensaje(client_socket, logger);
		if(mensaje == NULL) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje");
			return;
		}
		int insert_error = mensaje->header.error;
		destruirMensaje(mensaje);
		if(insert_error != 0) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo insertar en lis correctamente");
		}

		close(client_socket);
	}
}

int conectar_a_memoria(t_tipoSeeds* memoria) {

	log_info(logger, "Conectando a Memoria: %d", memoria->numeroMemoria);
	int puerto = atoi(memoria->puerto);
	return conectar_a_servidor(memoria->ip, puerto, mem);
}

void remover_memoria(t_tipoSeeds* memoria) {

	//Busco en mem_asociadas
	pthread_mutex_lock(&mutex_asociadas);
	int size_asociadas = list_size(mem_asociadas)-1;
	int posAsoc = 0;
	for (int i = size_asociadas; i >= 0; i--) {
		t_tipoSeeds *mem = list_get(mem_asociadas, i);
		log_info(logger, "REMOVE | MEMORIAS ASOCIADAS %d",
				mem->numeroMemoria);
		if (memoria->numeroMemoria == mem->numeroMemoria) {
			posAsoc = i;
			log_info(logger, "REMOVE | POS: %d Memoria %d de memorias asociadas",posAsoc,
					memoria->numeroMemoria);
			free(list_remove(mem_asociadas, posAsoc));
			log_info(logger, "LIST REMOVE | Memoria Posicion %d de memorias asociadas",posAsoc);
			break;
		}
	}
	pthread_mutex_unlock(&mutex_asociadas);

	//Busco en SC
	if(memoria_sc != NULL){
		if (memoria->numeroMemoria == memoria_sc->numeroMemoria) {
			pthread_mutex_lock(&mutex_memoria_sc);
			memoria_sc = NULL;
			log_info(logger, "REMOVE | Memoria %d de memoria SC",
					memoria->numeroMemoria);
			pthread_mutex_unlock(&mutex_memoria_sc);
		}
	}

	//Busco en SHC
	pthread_mutex_lock(&mutex_memoria_shc);
	int size_shc = list_size(lista_criterio_shc)-1;
	int posSHC = 0;
	for (int i = size_shc; i >= 0; i--) {
		t_tipoSeeds *mem = list_get(lista_criterio_shc, i);
		if (memoria->numeroMemoria == mem->numeroMemoria) {
			//Si la encontro aca, remover del diccionario
			posSHC = i;
			log_info(logger, "REMOVE | Memoria %d de memorias SHC",
					memoria->numeroMemoria);
			//remueve de hashdictionary
			remove_memoria_hashdictionary(memoria->numeroMemoria);
			free(list_remove(lista_criterio_shc, posSHC));
			break;
		}
	}
	pthread_mutex_unlock(&mutex_memoria_shc);

	//Busco en EC
	pthread_mutex_lock(&mutex_memoria_ev);
	int size_ev = list_size(lista_criterio_ev)-1;
	int posEC = 0;
	for (int i = size_ev; i >= 0; i--) {
		t_tipoSeeds *mem = list_get(lista_criterio_ev, i);
		if (memoria->numeroMemoria == mem->numeroMemoria) {
			posEC = i;
			log_info(logger, "REMOVE | Memoria %d de memorias EC",
					memoria->numeroMemoria);
			free(list_remove(lista_criterio_ev, posEC));
			break;
		}
	}
	pthread_mutex_unlock(&mutex_memoria_ev);

}

void remove_memoria_hashdictionary(int numeroMemoria) {

	for (int i = 0; 65535 > i; i++) {

		char *key = string_itoa(i);

		int * memoria_dic = dictionary_get(hashdictionary, key);
		if(memoria_dic == NULL){
			free(key);
			continue;
		}
		if (* memoria_dic == numeroMemoria) {
			free(dictionary_remove(hashdictionary, key));
			log_info(logger, "REMOVE| Memoria %d key %s de diccionario hash",
					numeroMemoria, key);
		}

		free(key);
	}

}


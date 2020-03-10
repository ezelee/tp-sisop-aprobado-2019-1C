/*
 * Metadata.c
 *
 *  Created on: 20 jul. 2019
 *      Author: utnso
 */
#include "Metadata.h"

void aplicar_tiempo_refresh() {

	sleep(kernel_conf.metadata_refresh / 1000);
}

void actualizar_metadata() {

	log_info(logger, "REFRESH| Iniciando.");

	t_tipoSeeds *memoria = get_memoria_conectada();

	log_info(logger, "REFRESH| Memoria asignada: %d", memoria->numeroMemoria);
	int cliente = conectar_a_memoria(memoria);

	if(cliente < 0){
		log_info(logger, "REFRESH| ERR - NO PUDO CONECTARSE A MEM.");
		return;
	}

	// Solicitud
	describe_global(cliente);
	log_info(logger, "REFRESH| Finalizado.");
	close(cliente);
}

void guardar_metadata(char *buffer) {
	//nombre_tabla;tipoConsistencia;particiones;compactationTime
	// PERSONAS;SHC;5;1000

	log_info(logger, "REFRESH| Guardo Metadata");
	char **elementos = string_split(buffer, ";");

//	pthread_mutex_lock(&mutex_metadata);
	t_metadata *metadata = malloc(sizeof(t_metadata));
	strcpy(metadata->nombreTabla, elementos[0]);
	strcpy(metadata->tipoConsistencia, elementos[1]);
	metadata->particiones = atoi(elementos[2]);
	metadata->compactationTime = atoi(elementos[3]);
//	pthread_mutex_lock(&mutex_metadata);
	list_add(lista_metadata, metadata);
//	pthread_mutex_unlock(&mutex_metadata);
//	pthread_mutex_unlock(&mutex_metadata);

	log_info(logger, "REFRESH| Cantidad de Metadatas: %d", lista_metadata->elements_count);

	int i = 0;
	while (elementos[i] != NULL) {
		free(elementos[i]);
		i++;
	}
	free(elementos);

}

void limpiar_metadata() {
//	pthread_mutex_lock(&mutex_metadata);
	if (lista_metadata != NULL) {
//		pthread_mutex_lock(&mutex_metadata);
		int size_metadata = list_size(lista_metadata);
		for (int i = 0; i < size_metadata; i++) {
			t_metadata *metadata = list_get(lista_metadata, i);
			free(metadata);
		}
		list_clean(lista_metadata);
//		pthread_mutex_unlock(&mutex_metadata);
	}
//	pthread_mutex_unlock(&mutex_metadata);
}

void describe_global(int cliente) {
	int cantidad = 0;
	log_info(logger, "DESCRIBE| Inicio de DESCRIBE");
	enviarMensaje(kernel, describe_global_, 0, NULL, cliente, logger, mem);
	t_mensaje* mensajeCantidad = recibirMensaje(cliente, logger);
	if (mensajeCantidad == NULL) {
		loggear(logger, LOG_LEVEL_ERROR,"No se pudo recibir mensaje de mem");
		return;
	}
	cantidad = mensajeCantidad->header.error;
	log_info(logger, "METADATA| La cantidad de tablas es: %d", cantidad);
	destruirMensaje(mensajeCantidad);

	int longAcum = 0;

	pthread_mutex_lock(&mutex_metadata);
	limpiar_metadata();
	while (cantidad-- > 0) {

		t_mensaje* mensaje = recibirMensaje(cliente, logger);
		if (mensaje == NULL) {
			loggear(logger, LOG_LEVEL_ERROR,
					"No se pudo recibir mensaje de mem");
			pthread_mutex_unlock(&mutex_metadata);
			return;
		}
		log_info(logger, "DESCRIBE| Mensaje recibido: %d", cantidad);
		char* buffer_describe= string_new();
		longAcum += mensaje->header.longitud;
		string_append(&buffer_describe, mensaje->content);

		log_info(logger, "DESCRIBE| Metadata: %s", buffer_describe);

		guardar_metadata(buffer_describe);
		destruirMensaje(mensaje);
		free(buffer_describe);
	}
	pthread_mutex_unlock(&mutex_metadata);
}

int validar_tabla(char *nombre){
	bool findMd(void* element) {
			t_metadata *metadata = element;
			return string_equals_ignore_case(nombre, metadata->nombreTabla);
		}
	pthread_mutex_lock(&mutex_metadata);
	int result = list_any_satisfy(lista_metadata, &findMd);
	pthread_mutex_unlock(&mutex_metadata);

	return result;
}

t_metadata* buscar_tabla(char *nombre) {

	t_metadata* tablaEncontrada=NULL;
	int esLaTabla(t_metadata *tabla) {
		return string_equals_ignore_case(nombre, tabla->nombreTabla);
	}

	loggear(logger, LOG_LEVEL_INFO, "Buscando %s en Metadata", nombre);

	pthread_mutex_lock(&mutex_metadata);
	t_metadata* tabla = list_find(lista_metadata, (void*) esLaTabla);
	if(tabla!=NULL){
		tablaEncontrada = malloc(sizeof(t_metadata));
		memcpy(tablaEncontrada,tabla,sizeof(t_metadata));
	}
	pthread_mutex_unlock(&mutex_metadata);

	return tablaEncontrada;
}

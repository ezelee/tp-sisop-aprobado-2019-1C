/*
 * Conexion.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso Registro key:
 */
#include "Conexion.h"
#include "Configuracion.h"
#include "LFS.h"
#include "Error.h"
#include "errno.h"

void *listen_connexions() {

	int socket_lfs = definirSocket(logger);
	if (bindearSocketYEscuchar(socket_lfs, "127.0.0.1", atoi(lfs_conf.puerto),
			logger) <= 0)
		_exit_with_error("BIND", NULL);

	fd_set set_master, set_copia;
	FD_ZERO(&set_master);
	FD_SET(socket_lfs, &set_master);
	int descriptor_mas_alto = socket_lfs;

	while (true) {
		set_copia = set_master;
		int i = select(descriptor_mas_alto + 1, &set_copia, NULL, NULL, NULL);
		if (i == -1 && errno != EINTR)
			_exit_with_error("SELECT", NULL);

		int n_descriptor = 0;
		while (n_descriptor <= descriptor_mas_alto) {
			if (FD_ISSET(n_descriptor, &set_copia)) {
				//ACEPTAR CONXIONES
				if (n_descriptor == socket_lfs) {
					aceptar(socket_lfs, &descriptor_mas_alto, &set_master);
				}
				//PROCESAR MENSAJE
				else {
					procesar(n_descriptor, &set_master);
				}
			}
			n_descriptor++;
		}
	}

	//return socket_lfs;
}

void aceptar(int socket_lfs, int* descriptor_mas_alto, fd_set* set_master) {
	int client_socket;
	if ((client_socket = aceptarConexiones(socket_lfs, logger)) == -1) {
		loggear(logger, LOG_LEVEL_ERROR, "Error en el accept");
		return;
	}
	FD_SET(client_socket, set_master);
	if (client_socket > *descriptor_mas_alto) {
		*descriptor_mas_alto = client_socket;
	}
}

void procesar(int n_descriptor, fd_set* set_master) {
	t_mensaje* msg;

	loggear(logger, LOG_LEVEL_INFO, "Recibiendo mensaje...");

	if ((msg = recibirMensaje(n_descriptor, logger)) == NULL) {
		close(n_descriptor);
		FD_CLR(n_descriptor, set_master);
		destruirMensaje(msg);
		return;
	}

	loggear(logger, LOG_LEVEL_INFO, "Proceso: %d", msg->header.tipoProceso);
	loggear(logger, LOG_LEVEL_INFO, "Mensaje: %d", msg->header.tipoMensaje);

	switch (msg->header.tipoProceso) {
	case mem:
		;
		switch (msg->header.tipoMensaje) {

		case handshake:
			log_debug(logger, "Handshake.");
			enviarMensaje(lis, handshake, sizeof(lfs_conf.tamano_value),
					&lfs_conf.tamano_value, n_descriptor, logger, mem);
			break;

		case insert:
			log_debug(logger, "Se recibió un insert");
			t_insert *msginsert = malloc(sizeof(t_insert));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",
					msg->header.longitud);
			memcpy(msginsert, msg->content, msg->header.longitud);

			loggear(logger, LOG_LEVEL_INFO, "Nombre Tabla :%s",
					msginsert->nombreTabla);

			loggear(logger, LOG_LEVEL_INFO, "Timestamp :%d",
					msginsert->timestamp);

			loggear(logger, LOG_LEVEL_INFO, "Key :%d", msginsert->key);

			loggear(logger, LOG_LEVEL_INFO, "Value :%s", msginsert->value);

			pthread_mutex_lock(&mutex_compactacion);
			int resultadoInsert = InsertarTabla(msginsert);
			pthread_mutex_unlock(&mutex_compactacion);

			loggear(logger, LOG_LEVEL_WARNING, "Resultado insert :%d",
					resultadoInsert);
			aplicar_retardo();
			enviarMensajeConError(lis, insert, 0, NULL, n_descriptor, logger,
					mem, resultadoInsert);
			free(msginsert);
			break;

		case create:
			log_debug(logger, "Se recibió mensaje create");
			t_create *msgCreate = malloc(sizeof(t_create));
			memcpy(msgCreate, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO,
					"Tabla %s con consistencia %s, %d particiones, %d tiempo de compactacion",
					msgCreate->nombreTabla, msgCreate->tipo_cons,
					msgCreate->num_part, msgCreate->comp_time);

			int resultadoCreate = CrearTabla(msgCreate);
			loggear(logger, LOG_LEVEL_WARNING, "Resultado create tabla %s:%d",
					msgCreate->nombreTabla, resultadoCreate);
			aplicar_retardo();
			enviarMensajeConError(lis, insert, 0, NULL, n_descriptor, logger,
					mem, resultadoCreate);
			free(msgCreate);

			break;

		case drop:
			log_debug(logger, "Se recibió mensaje drop");

			t_drop *dropTabla = malloc(sizeof(t_drop));
			memcpy(dropTabla, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "Eliminando tabla %s",
					dropTabla->nombreTabla);
			int resultadoDrop = DropearTabla(dropTabla->nombreTabla);
			loggear(logger, LOG_LEVEL_WARNING, "Resultado drop tabla %s:%d",
					dropTabla->nombreTabla, resultadoDrop);
			aplicar_retardo();
			enviarMensajeConError(lis, insert, 0, NULL, n_descriptor, logger,
					mem, resultadoDrop);

			free(dropTabla);
			break;

		case selectMsg:
			log_debug(logger, "Se recibió mensaje select");
			t_select *selectMensaje = malloc(sizeof(t_select));
			memcpy(selectMensaje, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "Buscando key: %d en tabla: %s",
					selectMensaje->key, selectMensaje->nombreTabla);
			pthread_mutex_lock(&mutex_compactacion);
			t_registro *resultado = BuscarKey(selectMensaje);
			pthread_mutex_unlock(&mutex_compactacion);
			aplicar_retardo();
			if (resultado != NULL && resultado->key != -1) {
				enviarMensajeConError(lis, selectMsg, sizeof(t_registro),
						resultado, n_descriptor, logger, mem, 0);
			} else {
				enviarMensajeConError(lis, selectMsg, 0, NULL, n_descriptor,
						logger, mem, -1);
			}


			free(selectMensaje);
			free(resultado);
			break;

		case describe:
			log_debug(logger, "Se recibió mensaje describe");
			t_describe *describeMensaje = malloc(sizeof(t_describe));
			memcpy(describeMensaje, msg->content, msg->header.longitud);
			if (string_is_empty(describeMensaje->nombreTabla)) {
				loggear(logger, LOG_LEVEL_INFO,
						"Buscando metadata para todas las tablas");
				//recorrer tablasGlobal y enviar metadata por tabla
				int cantTablas = list_size(tablasGlobal);
				log_info(logger, "countTables: %d", cantTablas);
				for (int i = 0; i < cantTablas; i++) {
					t_tcb *tabla = list_get(tablasGlobal, i);
					t_metadata *metadata;
					metadata = ObtenerMetadataTabla(tabla->nombre_tabla);

					char *describeGlobal = descomponer_metadata(metadata,
							tabla->nombre_tabla);
					log_info(logger, "Enviando describe: %s", describeGlobal);
					aplicar_retardo();
					enviarMensajeConError(lis, describe,
							(strlen(describeGlobal) + 1), describeGlobal,
							n_descriptor, logger, mem, 0);
					free(describeGlobal);
					free(metadata);
				}
				log_debug(logger, "Fin mensaje describe");
			} else if (ExisteTabla(describeMensaje->nombreTabla)) {

				log_info(logger, "Buscando metadata para tabla %s",
						describeMensaje->nombreTabla);
				t_metadata *metadata;
				metadata = ObtenerMetadataTabla(describeMensaje->nombreTabla);
				char *describeTabla = descomponer_metadata(metadata, describeMensaje->nombreTabla);
				log_info(logger, "Enviando describe: %s", describeTabla);
				aplicar_retardo();
				enviarMensajeConError(lis, describe,
						(strlen(describeTabla) + 1), describeTabla,
						n_descriptor, logger, mem, 0);

				free(metadata);
				free(describeTabla);
			} else {
				enviarMensajeConError(lis, describe, 0, NULL, n_descriptor,
						logger, mem, -1);
			}
			free(describeMensaje);
			break;
		case countTables:
			log_debug(logger, "Mensaje countTables recibido");
			int cantidadTablas = list_size(tablasGlobal);
			log_info(logger, "countTables: %d",cantidadTablas);
			aplicar_retardo();
			enviarMensajeConError(lis, countTables, 0, NULL, n_descriptor,
					logger, mem, cantidadTablas);

		}
		destruirMensaje(msg);
		break;
	default:
		;
	}

}


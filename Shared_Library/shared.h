/*
 * shared.h
 *
 *  Created on: 22 sep. 2018
 *      Author: utnso
 */

#ifndef SHARED_H_
#define SHARED_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

//#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define VALUE 128

/* Estructuras de Hackers */
typedef enum tipoMensaje {
	handshake,
	gossipingMsg,
	describe,
	selectMsg,
	insert,
	create,
	drop,
	journal,
	dump,
	countTables,
	salir,
	describe_global_
}t_tipoMensaje;

typedef enum tipoProceso {
	lis, mem, kernel
} t_tipoProceso;

typedef struct {
	t_tipoProceso 	tipoProceso;
	t_tipoMensaje 	tipoMensaje;
	int32_t 		longitud;
	int32_t 		error;
}__attribute__((packed)) t_header;

typedef struct {
	t_header 		header;
	void* 			content;
}__attribute__((packed)) t_mensaje;


//Structs para el paso de mensajes

//describe
typedef struct {
	char 		nombreTabla[50];
	int			id_proceso;
}__attribute__((packed)) t_describe;

//select
typedef struct {
	char 		nombreTabla[50];
	int 		key;
	int			id_proceso;
}__attribute__((packed)) t_select;

//insert
typedef struct {
	char 		nombreTabla[50];
	unsigned long long 		timestamp;
	int 		key;
	char 		value[VALUE];
	int			id_proceso;
}__attribute__((packed)) t_insert;

//create
typedef struct {
	char 		nombreTabla[50];
	char 		tipo_cons[4];
	int 		num_part;
	int 		comp_time;
	int			id_proceso;
}__attribute__((packed)) t_create;

//drop
typedef struct {
	char 		nombreTabla[50];
	int			id_proceso;
}__attribute__((packed)) t_drop;

//registro
typedef struct {
	unsigned long long timestamp;
	int key;
	char value[VALUE];
}__attribute__((packed)) t_registro;

/* Definición */

/*funciones de log*/
t_log* configurar_logger(char* nombreLog, char* nombreProceso);
t_log* configurar_logger_verbose(char* nombreLog, char* nombreProceso, bool verbose);
void loggear(t_log* logger, t_log_level level, const char* message_format, ...);
void destruir_logger(t_log* logger);

/*funciones de configuracion*/
t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger);
void remove_quotes(char *line);
void destroy_config(t_config* config);

/* funciones de socket */
int definirSocket(t_log* logger);
int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger);
int aceptarConexiones(int socket, t_log* logger);

int conectarseAServidor_w_to(int socket, char* ip, int puerto, t_log* logger);

int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger);
int enviarMensaje(int tipoProcesoEmisor, int tipoMensaje, int len, void* content, int socketReceptor, t_log* logger, int tipoProcesoReceptor);
int enviarMensajeConError(int tipoProcesoEmisor, int tipoMensaje, int len, void* content, int socketReceptor, t_log* logger, int tipoProcesoReceptor, int error);
t_mensaje* recibirMensaje(int socketEmisor, t_log* logger);
void destruirMensaje(t_mensaje* msg);
void destruirBuffer(void* buffer);

/* Auxiliares */
void* serializar(int tipoProceso, int tipoMensaje, int len, void* content);
void* serializarConError(int tipoProceso, int tipoMensaje, int len, void* content, int error);
t_mensaje* deserializar(void* buffer);

unsigned long long obtenerTimeStamp();
char** strlineassplit(const char* s, const char* del);
char* get_nombre_proceso(int enum_proceso);

#endif /* SHARED_H_ */

/*
 * gossiping.h
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#ifndef GOSSIPING_H_
#define GOSSIPING_H_

#include "shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define NUM_CONEX 100
#define DESCONOCIDO -1
#define DESCONECTADO 0
#define CONECTADO 1

pthread_t cliente;
pthread_mutex_t mutexGossiping;
pthread_mutex_t mutexprocessGossiping;
t_list *LISTA_CONECTADOS;

typedef struct tipoSeeds {
	int numeroMemoria;
	char ip[30];
	char puerto[15];
	char estado;
} t_tipoSeeds;

typedef enum tipoServidor {
	memoria,gossiping,kernelGoss
} t_tipoServidor;

/*
 * Obtiene IP local si no esta en archivo de configuracion
 */
char* getLocalIp(char *MEM_CONF_IP);

/*
 * Arma la lista de seeds que envia por msj
 */
char *armarMensajeListaSEEDSStruct(t_log *logger,t_list *LISTA_CONN);

/*
 * Procesa Msj recibido de Gossiping
 */
int procesarMsjGossipingStruct(char *mensaje, char *primerParser, char *segundoParser, t_log *logger,t_list *LISTA_CONN);

/*
 * KERNEL - MEMORIA
 * Proceso del hilo Gossiping
 * parametros
 * loogger: logger del proceso
 * LISTA_CONN: lista en MEMORIA de SEEDS del proceso
 */
void processGossipingStruct(t_log *logger,t_list *LISTA_CONN, char tipoProceso);

/*
 * KERNEL - MEMORIA
 * Crea una lista de Struct de SEEDS, si es KERNEL no ingresa su propia IP
 * Parametros
 * tipoProceso: el proceso que invoca la funcion
 * MEM_CONF_IP: IP del proceso principal del archivo de configuracion, para KERNEL no es necesario
 * MEM_CONF_PUERTO: PUERTO del proceso principal del archivo de configuracion, para KERNEL no es necesario
 * MEM_CONF_NUMERO_MEMORIA: Numero de la memoria principal, para KERNEL enviar DESCONOCIDO
 * MEM_CONF_IP_SEEDS: Lista de SEEDS de archivo de configuracion
 * MEM_CONF_PUERTO_SEEDS: Lista de PUERTOS SEEDS de archivo de configuracion
 * logger: Logger del proceso
 * LISTA_CONN: Lista en Memoria de los SEEDS
 */
int crearListaSeedsStruct(char tipoProceso,char *MEM_CONF_IP,char *MEM_CONF_PUERTO,int MEM_CONF_NUMERO_MEMORIA, char **MEM_CONF_IP_SEEDS, char **MEM_CONF_PUERTO_SEEDS, t_log *logger,t_list *LISTA_CONN);

#endif /* GOSSIPING_H_ */

/*
 * connection.h
 *
 *  Created on: 21 sep. 2018
 *      Author: utnso
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <shared.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netdb.h> // Para getaddrinfo
#include <arpa/inet.h> //Para inet_addr
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

int socket_lis;

int connect_to_server(char* IP, char* PUERTO, int proceso, int flag);
int listen_connexions(char* PUERTO);

#endif /* CONNECTION_H_ */

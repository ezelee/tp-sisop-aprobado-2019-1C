/*
 * Conexion.h
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "LFS.h"
#include <stdio.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netdb.h> // Para getaddrinfo
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

int socket_lfs;
void aceptar(int socket_lfs, int* descriptor_mas_alto, fd_set* set_master);
void *listen_connexions();
void procesar(int n_descriptor, fd_set* set_master);


#endif /* CONNECTION_H_ */

/*
 * InotifyLfs.h
 *
 *  Created on: 14 jul. 2019
 *      Author: utnso
 */

#ifndef HILOS_H_
#define HILOS_H_

#include "LFS.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

pthread_t thread_inotify;
pthread_t thread_consola;
pthread_t thread_conexiones;
pthread_t thread_dump;


int CrearHiloInotify();
void *crearInotify();
void CrearHiloConsola();
void CrearHiloConexiones();
void CrearHiloDump();

#endif /* HILOS_H_ */

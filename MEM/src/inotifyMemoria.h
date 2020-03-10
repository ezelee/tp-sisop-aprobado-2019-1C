/*
 * inotify.h
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#ifndef INOTIFYMEMORIA_H_
#define INOTIFYMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include <limits.h>
#include <signal.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

pthread_t inotify;

int crearHiloInotify();

#endif /* INOTIFYMEMORIA_H_ */

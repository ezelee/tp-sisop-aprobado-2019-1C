/*
 * LISSANDRA.h
 *
 *  Created on: 29 abr. 2019
 *      Author: utnso
 */

#ifndef LFS_H_
#define LFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <shared.h>
#include <parser.h>
#include <string.h>

#include "Configuracion.h"
#include "Filesystem.h"
#include "Consola.h"
#include "Conexion.h"
#include "Dump.h"
#include "Error.h"
#include "Compactacion.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <sys/inotify.h>
#include <limits.h>
#include <signal.h>
#include "Hilos.h"

t_log *logger;
t_list *memtable;
t_list *tablasGlobal;

typedef struct {
	char nombre_tabla[20];
     t_list *lista;
}__attribute__((packed)) t_tabla;



typedef struct {
	char nombre_tabla[20];
	int bloqueado;
	int contadorTmp;
}__attribute__((packed)) t_tcb;

pthread_mutex_t mutex_temp;
pthread_mutex_t mutex_compactacion;
pthread_mutex_t mutex_bitmap;

void Inicializar();
t_tabla* ObtenerTabla(char *nombre);
void AlocarTabla(char *tabla, t_registro *registro);




#endif /* LFS_H_ */

/*
 * proceso.h
 *
 *  Created on: 7 jun. 2019
 *      Author: utnso
 */

#ifndef PROCESO_H_
#define PROCESO_H_

#include <shared.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <time.h>

int proceso_select(char* tabla, int clave, char** buffer, int* largo_buffer);
int proceso_insert(char* tabla, int clave, char* value, unsigned long long tstamp);
int proceso_create(char* tabla,char* tipo_cons, int num_part, int compact_time);
int proceso_describe(char* tabla, char** buffer, int* largo_buffer);
int proceso_journal();
int proceso_drop(char* tabla);

#endif /* PROCESO_H_ */

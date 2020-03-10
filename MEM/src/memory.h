/*
 * memory.h
 *
 *  Created on: 31 oct. 2018
 *      Author: utnso
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <shared.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <time.h>

typedef struct {
	int frame;
	unsigned char modificado;
	unsigned long long timestamp;
} t_paginas_spa;

typedef struct {
	t_list* pag_lista;
} t_segmentos_spa;

typedef struct {
	char* path_tabla;
	t_list* seg_lista;
} t_adm_tabla_segmentos_spa;

typedef struct {
	char* path_tabla;
	int pagina;
} t_adm_tabla_frames_spa;

char* frames_spa;
int frames_spa_count;
int frame_spa_size;

t_list* adm_spa_lista;
t_list* adm_frame_lista_spa;

char* componer_registro(unsigned long long timestamp, unsigned int key, char* value, int largo_value);
t_registro* descomponer_registro(char *buffer);
void destruir_registro(t_registro* registro);

void liberar_memory_spa();
void init_memory_spa();
char* leer_bytes_spa(char* path_table, int segmento, int offset, int size);
int escribir_bytes_spa(char* path_table, int segmento, int offset, int size, char* buffer, unsigned char modificado);
int add_spa(char* path_table, int n_frames, unsigned long long timestamp);
void free_spa(char* path_table, int segmento);
void update_administrative_register_adm_table_spa(t_adm_tabla_segmentos_spa* adm_table);
void dump_memory_spa(char* path_table);
t_adm_tabla_frames_spa getPaginaMenorTimestamp();
int getPaginaForKey(char *path_table, unsigned int key);
int getSizePagesForTable(char *path_table);

#endif /* MEMORY_H_ */

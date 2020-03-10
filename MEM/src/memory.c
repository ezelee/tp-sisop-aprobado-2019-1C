/*
 * memory.c
 *
 *  Created on: 31 oct. 2018
 *      Author: utnso
 */
#include "MEM.h"
#include "shared.h"

#include "memory.h"
#include "file_conf.h"
/*
 * ---------------AUXILIARES--------------
 */

char* componer_registro(unsigned long long timestamp, unsigned int key, char value[VALUE], int largo_value)
{
	char* buffer = malloc(sizeof(timestamp)+sizeof(key)+tamanio_value);
	memset(buffer, 0x00, sizeof(timestamp)+sizeof(key)+tamanio_value);
	memcpy(buffer,&timestamp,sizeof(timestamp));
	memcpy(buffer+sizeof(timestamp),&key,sizeof(key));
	memcpy(buffer+sizeof(timestamp)+sizeof(key),&value[0],largo_value);
	return buffer;
}

t_registro* descomponer_registro(char *buffer)
{
	t_registro* registro = malloc(sizeof(t_registro));
	memcpy(&registro->timestamp,buffer,sizeof(unsigned long long));
	memcpy(&registro->key,buffer+sizeof(unsigned long long),sizeof(int));
	memcpy(&registro->value[0],buffer+sizeof(unsigned long long)+sizeof(int),tamanio_value);
	return registro;
}

void destruir_registro(t_registro* registro)
{
	free(registro);
	return;
}

bool exists_table_spa(char* path_table){
	int i;
	for(i=0; i<list_size(adm_frame_lista_spa);i++){
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
		if (string_equals_ignore_case(adm_table->path_tabla, path_table)) return true;
	}
	return false;
}

int get_free_frame() {
	int i;
	for(i=0; i<list_size(adm_frame_lista_spa); i++) {
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa,i);
		if(string_equals_ignore_case(adm_table->path_tabla, ""))
			return i;
	}
	return -1;
}

bool has_available_frames_spa(int n_frames) {
	bool find(void* element) {
		t_adm_tabla_frames_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla, "");
	}
	t_list* available_pages_list = list_filter(adm_frame_lista_spa, &find);
	int available_pages = list_size(available_pages_list);
	list_destroy(available_pages_list);
	return available_pages >= n_frames;
}

/*
unsigned int hash(int pid, int page) {
	char* str = malloc(strlen(string_itoa(pid)));
	strcpy(str, string_itoa(pid));
	strcat(str, string_itoa(page));
	unsigned int indice = atoi(str) % frames_seg_count;
	free(str);
	return indice;
}
*/

void clean_frame_spa(int frame) {
	int start = frame * frame_spa_size;
	int end = start + frame_spa_size;
	int i;
	for (i = start; i < end; i++) {
		frames_spa[i] = '#';
	}
}

void mem_allocate_fullspace_spa() {

	int mem_size = MEM_CONF.TAM_MEM;
	frames_spa = malloc(mem_size);
	memset(frames_spa, '#', mem_size);
}

void mem_desallocate_fullspace_spa() {

	free(frames_spa);
}
/*
 * -----------------------------------------
 */

void liberar_memory_spa() {

	mem_desallocate_fullspace_spa();
	int i,j,k;
	for (i = 0; i < frames_spa_count; i++) {
		t_adm_tabla_frames_spa* adm_table_frame = list_get(adm_frame_lista_spa,0);
		free(adm_table_frame->path_tabla);
		free(list_remove(adm_frame_lista_spa,0));
	}
	if(adm_frame_lista_spa!=NULL) list_destroy(adm_frame_lista_spa);

	int countSegLista = (adm_spa_lista==NULL)?0:list_size(adm_spa_lista);
	for (i = 0; i < countSegLista; i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_remove(adm_spa_lista,0);
		int countSeg = list_size(adm_table->seg_lista);
		for (j = 0; j < countSeg; j++) {
			t_segmentos_spa* adm_table_seg = list_remove(adm_table->seg_lista,0);
			int countPag = list_size(adm_table_seg->pag_lista);
			for (k = 0; k < countPag; k++) {
				free(list_remove(adm_table_seg->pag_lista,0));
			}
			list_destroy(adm_table_seg->pag_lista);
			free(adm_table_seg);
		}
		free(adm_table->path_tabla);
		list_destroy(adm_table->seg_lista);
		free(adm_table);
	}
	if(adm_spa_lista!=NULL) list_destroy(adm_spa_lista);

	frames_spa = NULL;
	frames_spa_count = 0;
	frame_spa_size = 0;

	adm_spa_lista = NULL;
	adm_frame_lista_spa = NULL;
}

void init_memory_spa() {

	int i;

	frame_spa_size = tamanio_value + sizeof(unsigned long long) + sizeof(int);
	frames_spa_count = MEM_CONF.TAM_MEM / frame_spa_size;

	mem_allocate_fullspace_spa();

	adm_frame_lista_spa = list_create();

	for (i = 0; i < frames_spa_count; i++) {
		t_adm_tabla_frames_spa* adm_table = malloc(sizeof(t_adm_tabla_frames_spa));
		adm_table->path_tabla = string_new();
		adm_table->pagina = -1;
		list_add(adm_frame_lista_spa, adm_table);
	}

	adm_spa_lista = list_create();

	/*for (i = 0; i < frames_seg_count; i++) {
		t_adm_tabla_segmentos* adm_table = malloc(sizeof(t_adm_tabla_segmentos));
		adm_table->pid = -1;
		adm_table->seg_lista = list_create();

		list_add(adm_frame_lista, adm_table);
	}*/
}

void free_spa(char* path_table, int pagina) {
	int i,j,k;

	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla, path_table);
	}

	bool find2(void* element) {

		bool find21(void* element) {
			t_paginas_spa* adm_table = element;
			return adm_table->frame != -1;
		}

		t_segmentos_spa* adm_table = element;
		return list_size(list_filter(adm_table->pag_lista,&find21));
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg = (adm_table!=NULL)?list_get(adm_table->seg_lista, 0):NULL;

	t_paginas_spa* adm_table_pag = (adm_table!=NULL)&&(adm_table_seg != NULL)?list_get(adm_table_seg->pag_lista, pagina):NULL;

	loggear(logger,LOG_LEVEL_DEBUG, "%s", (adm_table != NULL) && (adm_table_seg != NULL) && (adm_table_pag != NULL) ?
				"ENCONTRE PAGINA PARA LIBERAR!" : "NO ENCONTRE PAGINA PARA LIBERAR.");

	if(adm_table == NULL || adm_table_seg == NULL || adm_table_pag == NULL)
		return;

	t_adm_tabla_frames_spa* adm_table_frame = list_get(adm_frame_lista_spa,adm_table_pag->frame);

	free(adm_table_frame->path_tabla);
	adm_table_frame->path_tabla = string_new();
	adm_table_frame->pagina = -1;
	clean_frame_spa(adm_table_pag->frame);

	adm_table_pag->frame = -1;

	free(list_remove(adm_table_seg->pag_lista,pagina));

	/*int countPaginas = list_size(adm_table_seg->pag_lista);
	for (i = 0; i < countPaginas; i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista, 0);

		t_adm_tabla_frames_spa* adm_table_frame = list_get(adm_frame_lista_spa,adm_table_pag->frame);

		adm_table_frame->path_tabla = string_from_format("");
		adm_table_frame->segmento = 0;
		clean_frame_spa(adm_table_pag->frame);

		adm_table_pag->frame = -1;

		free(list_remove(adm_table_seg->pag_lista,0));
	}*/

//	if(list_size(list_filter(adm_table->seg_lista,&find2))==0) {
	if(list_size(adm_table_seg->pag_lista)==0) {
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "SE LIBERA LA TABLA.");
		int countSegLista = list_size(adm_spa_lista);
		for (i = 0; i < countSegLista; i++) {
			t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista,i);
			if(string_equals_ignore_case(adm_table->path_tabla,path_table)){
				int countSeg = list_size(adm_table->seg_lista);
				for (j = 0; j < countSeg; j++) {
					t_segmentos_spa* adm_table_spa = list_get(adm_table->seg_lista,0);
					int countPag = list_size(adm_table_spa->pag_lista);
					for (k = 0; k < countPag; k++) {
						free(list_remove(adm_table_spa->pag_lista,0));
					}
					list_destroy(adm_table_spa->pag_lista);
					free(adm_table_spa);
				}
				free(adm_table->path_tabla);
				list_destroy(adm_table->seg_lista);
				free(adm_table);
				list_remove(adm_spa_lista,i);
				break;
			}
		}
	}
}

int add_spa(char* path_table, int n_frames, unsigned long long timestamp) {
	loggear(logger,LOG_LEVEL_DEBUG, "segments-pages, table = '%s', n_frames = '%d'", path_table, n_frames);

	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla,path_table);
	}

	int known_segmentos = 0, known_paginas = 0, i;

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);
	known_segmentos = (adm_table == NULL)?0:list_size(adm_table->seg_lista);

	//t_segmentos_spa* adm_table_seg = adm_table->seg_lista;
	//known_paginas = list_size(adm_table_seg->pag_lista);

	if(!has_available_frames_spa(n_frames)) {
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "NO TENGO FRAMES DISPONIBLES.");
		return -1;
	}

	loggear(logger,LOG_LEVEL_DEBUG, "%s", "TENGO FRAMES DISPONIBLES!");

	t_adm_tabla_segmentos_spa* adm_table_new;
	t_segmentos_spa* adm_table_seg_new;

	if(known_segmentos==0) {
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "adm_table_seg_new!");
		adm_table_new =  malloc(sizeof(t_adm_tabla_segmentos_spa));
		adm_table_new->path_tabla = string_from_format(path_table);
		adm_table_new->seg_lista = list_create();
		list_add(adm_spa_lista, adm_table_new);

		adm_table = list_find(adm_spa_lista, &find);
		adm_table_seg_new = malloc(sizeof(t_segmentos_spa));
		adm_table_seg_new->pag_lista = list_create();
		list_add(adm_table->seg_lista,adm_table_seg_new);
	}

	/*adm_table = list_find(adm_spa_lista, &find);

	adm_table_seg_new = malloc(sizeof(t_segmentos_spa));
	adm_table_seg_new->pag_lista = list_create();
	list_add(adm_table->seg_lista,adm_table_seg_new);*/

	known_segmentos = (adm_table == NULL)?0:list_size(adm_table->seg_lista);

	adm_table_seg_new = list_get(adm_table->seg_lista, known_segmentos-1);
	known_paginas = (adm_table_seg_new == NULL)?0:list_size(adm_table_seg_new->pag_lista);

	//t_paginas_spa* adm_table_pag = list_get(adm_table_spa_new->pag_lista, known_paginas-1);
	for(i=0; i<n_frames; i++) {
		t_paginas_spa* adm_table_pag_new = malloc(sizeof(t_paginas_spa));
		adm_table_pag_new->frame = get_free_frame();
		adm_table_pag_new->timestamp = timestamp;
		adm_table_pag_new->modificado=0;
		list_add(adm_table_seg_new->pag_lista, adm_table_pag_new);
		known_paginas++;
		loggear(logger,LOG_LEVEL_DEBUG, "known_paginas %d",known_paginas);
		t_adm_tabla_frames_spa* adm_table_frames = list_get(adm_frame_lista_spa,adm_table_pag_new->frame);
		string_append_with_format(&adm_table_frames->path_tabla, path_table);
		adm_table_frames->pagina = known_paginas-1;
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "adm_table_pag_new!");
	}

	return known_paginas-1;
}

char* leer_bytes_spa(char* path_table, int segmento, int offset, int size) {
	sleep(MEM_CONF.RETARDO_MEM/1000);
	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla,path_table);
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg;

	do {
		adm_table_seg = list_get(adm_table->seg_lista,segmento);
		if(adm_table_seg==NULL)
			break;
		//segmento += (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? 1:0;
		offset = (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? offset-(list_size(adm_table_seg->pag_lista)*frame_spa_size):offset;
	} while(offset>(list_size(adm_table_seg->pag_lista)*frame_spa_size));

	loggear(logger,LOG_LEVEL_DEBUG, "Segmento %d, offset %d, size pag %d, frame spa %d",
				segmento, offset, list_size(adm_table_seg->pag_lista), frame_spa_size);
	adm_table_seg = list_get(adm_table->seg_lista,segmento);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
				(adm_table_seg != NULL) ? "ENCONTRE LA PAGINA!" : "NO ENCONTRE LA PAGINA.");

	char* buffer = malloc(size);
	memset(buffer, 0x00, size);
	if (adm_table_seg != NULL) {
		int page = offset/frame_spa_size, page_offset =  offset%frame_spa_size, start = 0;

		//for(i = page; i<list_size(adm_table_seg->pag_lista); i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,page);
		start = adm_table_pag->frame*frame_spa_size+page_offset;
		memcpy(buffer, frames_spa+start, size);
		//}
	}
	return buffer;
}

int escribir_bytes_spa(char* path_table, int segmento, int offset, int size, char* buffer, unsigned char modificado) {
	sleep(MEM_CONF.RETARDO_MEM/1000);
	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla,path_table);
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg;

	do {
		adm_table_seg = list_get(adm_table->seg_lista,segmento);
		if(adm_table_seg==NULL)
			break;
		//segmento += (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? 1:0;
		offset = (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? offset-(list_size(adm_table_seg->pag_lista)*frame_spa_size):offset;
	} while(offset>(list_size(adm_table_seg->pag_lista)*frame_spa_size));

	loggear(logger,LOG_LEVEL_DEBUG, "Segmento %d, offset %d, size pag %d, frame spa %d",
			segmento, offset, list_size(adm_table_seg->pag_lista), frame_spa_size);
	adm_table_seg = list_get(adm_table->seg_lista,segmento);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
				(adm_table_seg != NULL) ? "ENCONTRE LA PAGINA!" : "NO ENCONTRE LA PAGINA.");

	int start = 0;
	if (adm_table_seg != NULL) {
		int i, end = 0, b = 0;;
		int page = offset/frame_spa_size;
		int page_offset =  offset%frame_spa_size;

		//for (i = page; i < list_size(adm_table_seg->pag_lista); i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,page);
		start = adm_table_pag->frame*frame_spa_size+page_offset;
		end = start + size;
		for (i = start; i < end; i++) {
			frames_spa[i] = buffer[b];
			b++;
		}
		adm_table_pag->modificado = modificado;
		//}
	}
	else
		return -1;

	return start;
}

void dump_memory_spa(char* path_table) {
	int i;

	if(frames_spa == NULL) {
		printf("FAIL: No esta inicializada la memoria!\n");
		return;
	}

	if(path_table[0]!=0x00 && !exists_table_spa(path_table)) {
		printf("FAIL: No existe la tabla!\n");
		return;
	}

	if(path_table[0]==0x00){
		for (i = 0; i < list_size(adm_spa_lista); i++) {
			t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista, i);
			if (!string_equals_ignore_case(adm_table->path_tabla,""))
				printf("ACTIVE TABLE: %s\n",adm_table->path_tabla);
		}
		return;
	}

	char* dump_mem_struct = string_new();
	char* dump_act_process = string_new();
	char* dump_mem_content = string_new();

	if (string_equals_ignore_case(path_table,"")) {
		string_append(&dump_mem_content, frames_spa);
	} else {
		for (i = 0; i < list_size(adm_frame_lista_spa); i++) {
			t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
			if (string_equals_ignore_case(adm_table->path_tabla,path_table)) {
				void* frame = malloc(frame_spa_size);
				memset(frame,0x00,frame_spa_size);
				memcpy(frame,frames_spa+i*frame_spa_size, frame_spa_size);
				t_registro* registro = descomponer_registro(frame);
				string_append_with_format(&dump_mem_content,
						"FRAME: %d | TABLA: %s | SEGMENTO: %d\n[%llu][%d][%s]\n",
						i, adm_table->path_tabla, adm_table->pagina,
						registro->timestamp, registro->key,registro->value);
				free(frame);
				destruir_registro(registro);
			}
		}
	}

	for (i = 0; i < frames_spa_count; i++) {
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
		if (string_equals_ignore_case(adm_table->path_tabla,path_table)
				|| string_equals_ignore_case(path_table,"")) {
			string_append_with_format(&dump_mem_struct,
					"FRAME: %d | TABLE: %s | SEGMENTO: %d\n", i,
					adm_table->path_tabla, adm_table->pagina);
		}
	}

	for (i = 0; i < list_size(adm_spa_lista); i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista, i);
		if (!string_equals_ignore_case(adm_table->path_tabla,"")) {
			string_append_with_format(&dump_act_process, "ACTIVE TABLE: %s\n",
					adm_table->path_tabla);
		}
	}

	char* dump_total = string_new();
	string_append_with_format(&dump_total, "TABLA DE SEGMENTOS:\n%s\n",
			dump_mem_struct);
	string_append_with_format(&dump_total, "CONTENIDO MEMORIA:\n%s\n\n",
			dump_mem_content);
	string_append_with_format(&dump_total, "LISTADO DE TABLAS ACTIVAS:\n%s\n",
			dump_act_process);


	printf("%s\n", dump_total);
	loggear(logger,LOG_LEVEL_DEBUG,"\n>TABLE: %s\n\n%s\n", path_table, dump_total);

	free(dump_act_process);
	free(dump_mem_content);
	free(dump_mem_struct);
	free(dump_total);
}

t_adm_tabla_frames_spa getPaginaMenorTimestamp(char *pathTabla) {
	t_adm_tabla_frames_spa frame_spa = {.path_tabla = string_new(),.pagina=-1};
	unsigned long long timestamp = 0;
	int i,j;

	for (i = 0; i < list_size(adm_spa_lista); i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista, i);
		t_segmentos_spa* adm_table_seg = list_get(adm_table->seg_lista, 0);
		for (j = 0; j < list_size(adm_table_seg->pag_lista); j++) {
			t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,j);
			if( string_equals_ignore_case(adm_table->path_tabla,pathTabla) &&
					(timestamp==0 || adm_table_pag->timestamp < timestamp) && adm_table_pag->modificado == 0) {
				string_append_with_format(&frame_spa.path_tabla,adm_table->path_tabla);
				frame_spa.pagina = j;
				timestamp = adm_table_pag->timestamp;
				loggear(logger,LOG_LEVEL_ERROR,"devolviendo pagina %s %d",frame_spa.path_tabla , j);

			}
		}
	}

	return frame_spa;
}

int getPaginaForKey(char *path_table, unsigned int key) {
	int j;

	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla,path_table);
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
					(adm_table != NULL) ? "ENCONTRE EL SEGMENTO FOR KEY!" : "NO ENCONTRE EL SEGMENTO FOR KEY.");

	if(adm_table==NULL)
		return -2;

	t_segmentos_spa* adm_table_seg = list_get(adm_table->seg_lista, 0);

	for (j = 0; j < list_size(adm_table_seg->pag_lista); j++) {
		char* buffer = leer_bytes_spa(path_table, 0, j*frame_spa_size, frame_spa_size);
		t_registro* registro = descomponer_registro(buffer);
		int r_key = registro->key;
		destruir_registro(registro);
		free(buffer);
		loggear(logger,LOG_LEVEL_DEBUG, "Buscando: %d - Obtenido: %d", key, r_key);
		if(r_key == key)
			return j;
	}

	loggear(logger,LOG_LEVEL_DEBUG, "%s","NO ENCONTRE LA PAGINA FOR KEY!");
	return -1;
}

int getSizePagesForTable(char *path_table) {
	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return string_equals_ignore_case(adm_table->path_tabla,path_table);
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
					(adm_table != NULL) ? "ENCONTRE EL SEGMENTO FOR TABLA!" : "NO ENCONTRE EL SEGMENTO FOR TABLA.");

	if(adm_table==NULL)
		return -1;

	t_segmentos_spa* adm_table_seg = list_get(adm_table->seg_lista, 0);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
					(adm_table_seg != NULL) ? "ENCONTRE PAGINAS FOR KEY!" : "NO ENCONTRE PAGINAS FOR KEY.");

	if(adm_table_seg==NULL)
		return -1;

	return list_size(adm_table_seg->pag_lista);
}

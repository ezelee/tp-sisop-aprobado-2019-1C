/*
 * Criterios.c
 *
 *  Created on: 22 jul. 2019
 *      Author: utnso
 */

#include "Criterios.h"


char* criterio_str[] = {
		"SC"
		, "SHC"
		, "EC"
		, NULL
};

t_tipoCriterio criterio_to_enum(char *sval) {
	t_tipoCriterio result = SC;
	int i = 0;
	for (i = 0; criterio_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, criterio_str[i]))
			return result;
	return -1;
}

t_tipoSeeds* obtener_memoria_lista_SHC(int numero){

	int findMemory(t_tipoSeeds * memoria) {
		log_info(logger, "CRITERIO| CRITERIOS BUSQUEDA SHC | Memoria %d en LISTA_CONN", memoria->numeroMemoria);
		log_info(logger, "CRITERIO| CRITERIOS BUSQUEDA SHC | Numero buscado %d", numero);
		log_info(logger, "CRITERIO| CRITERIOS BUSQUEDA SHC | Igualdad: %d", memoria->numeroMemoria == numero);
		return memoria->numeroMemoria == numero;
	}

	log_info(logger, "CRITERIO| Buscando memoria %d en lista_criterio_shc", numero);
//	return list_find(LISTA_CONN, &findMemory);
	pthread_mutex_lock(&mutex_memoria_shc);
//	t_tipoSeeds* mem = list_get(lista_criterio_shc,numero);
	t_tipoSeeds* mem = list_find(lista_criterio_shc, (void *)findMemory);
	pthread_mutex_unlock(&mutex_memoria_shc);

	if (mem != NULL) {
		log_info(logger, "CRITERIO| CRITERIOS BUSQUEDA SHC | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	else
	{
		mem = malloc(sizeof(t_tipoSeeds));
		memset(mem, 0x00, sizeof(t_tipoSeeds));
		mem->numeroMemoria = -1;
		log_info(logger, "CRITERIO| CRITERIOS BUSQUEDA SHC NO ENCONTRADA | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	return mem;

}

t_tipoSeeds* obtener_memoria_lista(int numero){

	int findMemory(t_tipoSeeds * memoria) {
		log_info(logger, "CRITERIO| Memoria %d en LISTA_CONN", memoria->numeroMemoria);
		log_info(logger, "CRITERIO| Numero buscado %d", numero);
		log_info(logger, "CRITERIO| Igualdad: %d", memoria->numeroMemoria == numero);
		return memoria->numeroMemoria == numero;
	}


	log_info(logger, "CRITERIO| Buscando memoria %d en LISTA_CONN", numero);
//	return list_find(LISTA_CONN, &findMemory);
	pthread_mutex_lock(&mutex_LISTA_CONN);
	t_tipoSeeds* mem = list_find(LISTA_CONN, (void *)findMemory);
	pthread_mutex_unlock(&mutex_LISTA_CONN);

	if (mem != NULL) {
		log_info(logger, "CRITERIO| MEMMMMMMM | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	return mem;

}

int agregar_memoria_a_criterio(int nro_memoria, char* criterio) {

	int resultado = -1;

	// Valido si existe la memoria -- CHECKEAR SI SE NECESITA QUE ESTE CONECTADA
	t_tipoSeeds* memoria = obtener_memoria_lista(nro_memoria);

	if (memoria == NULL) {
		log_info(logger, "CRITERIO| La memoria %d no existe.", nro_memoria);
		return resultado;
	}

	int e_criterio = criterio_to_enum(criterio);

	switch(e_criterio){

		case SC:

			if(memoria_sc == NULL){

				pthread_mutex_lock(&mutex_memoria_sc);
				memoria_sc = malloc(sizeof(t_tipoSeeds));
				memcpy(memoria_sc, memoria,sizeof(t_tipoSeeds));
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
				pthread_mutex_unlock(&mutex_memoria_sc);

				resultado = 1;

			} else {
				log_info(logger, "CRITERIO| %s ya tiene memoria asignada.", criterio);
			}
			break;

		case SHC:

			pthread_mutex_lock(&mutex_memoria_shc);
			if (!existe_memoria(memoria, lista_criterio_shc)) {

				t_tipoSeeds* memoriaSHC = malloc(sizeof(t_tipoSeeds));
				memcpy(memoriaSHC, memoria,sizeof(t_tipoSeeds));

				list_add(lista_criterio_shc, memoriaSHC);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
				resultado = 1;
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}
			pthread_mutex_unlock(&mutex_memoria_shc);


			break;

		case EC:

			pthread_mutex_lock(&mutex_memoria_ev);
			if (!existe_memoria(memoria, lista_criterio_ev)) {

				t_tipoSeeds* memoriaEV = malloc(sizeof(t_tipoSeeds));
				memcpy(memoriaEV, memoria,sizeof(t_tipoSeeds));

				list_add(lista_criterio_ev, memoriaEV);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
				resultado = 1;
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}
			pthread_mutex_unlock(&mutex_memoria_ev);


			break;

		default:
			printf("CRITERIO| Criterio no reconocido.\n");
	}

	return resultado;
}

int enviarjournalSegunCriterio(char *criterio)
{
	int tipo_criterio = criterio_to_enum(criterio);
	int resultado = 1;
	switch (tipo_criterio) {
		case SC:
			enviar_journal_sc();

			break;
		case SHC:
			enviar_journal_shc();

			break;
		case EC:
			enviar_journal_ev();

			break;
	}
	return resultado;
}

t_tipoSeeds* get_memoria_por_criterio(char *criterio, int key) {
	t_tipoSeeds *memory;
	int tipo_criterio = criterio_to_enum(criterio);
	switch (tipo_criterio) {
		case SC:
			pthread_mutex_lock(&mutex_memoria_sc);
			if (memoria_sc == NULL) {
				memory = malloc(sizeof(t_tipoSeeds));
				memset(memory, 0x00, sizeof(t_tipoSeeds));
				memory->numeroMemoria = -1;
				pthread_mutex_unlock(&mutex_memoria_sc);
				return memory;
			}
			memory = memoria_sc;
			pthread_mutex_unlock(&mutex_memoria_sc);
			break;

		case SHC:;
			int nro_memoria = get_memory_hash_SHC(key);

			memory = obtener_memoria_lista_SHC(nro_memoria);

			//TODO: HACER JOURNALING A TODAS LAS MEMORIAS??

			break;

		case EC:
			// TODO
			//memoria random
			memory = obtener_memoria_random();
			if (memory->numeroMemoria <0)
			{
				log_info(logger, "CRITERIO| MEMORIA: %d",memory->numeroMemoria);
				return memory;
			}
			break;

	}
	return memory;
}

int existe_memoria(t_tipoSeeds* memoria, t_list* lista_criterio) {

	int existe_memoria(t_tipoSeeds* mem) {
		return mem->numeroMemoria == memoria->numeroMemoria;
	}

	return list_any_satisfy(lista_criterio, (void *) existe_memoria);
}

int get_memory_hash_SHC(int key) {

	char *key_convert = string_itoa(key);
	log_info(logger, "Hash GET| Key GET %s", key_convert);
	int * memoria_dic = dictionary_get(hashdictionary, key_convert);

	if (memoria_dic == NULL) {

		pthread_mutex_lock(&mutex_memoria_shc);
		int sizeLista = list_size(lista_criterio_shc);
		pthread_mutex_unlock(&mutex_memoria_shc);

		log_info(logger, "Hash| tamaño lista_criterio_shc  %d", sizeLista);

		if (sizeLista == 0)
			return 0;

		int randomMemoria = key % sizeLista;

		pthread_mutex_lock(&mutex_memoria_shc);
		t_tipoSeeds *memoryRandom = list_get(lista_criterio_shc,randomMemoria);
		pthread_mutex_unlock(&mutex_memoria_shc);

		int * memoria = malloc(sizeof(int));

		*memoria = memoryRandom->numeroMemoria;

		log_info(logger, "Hash Dictionary PUT| Key %d Memoria %d", key, *memoria);

		dictionary_put(hashdictionary, key_convert, memoria);
		memoria_dic = memoria;
	} else {

		log_info(logger, "Hash Dictionary GET| Key %d Memoria %d", key, *memoria_dic);
	}

	log_info(logger, "Hash Result| Key %d Memoria %d", key, *memoria_dic);
	free(key_convert);
	return *memoria_dic;
}

t_tipoSeeds * get_memoria_asociada() {
	int n;
	pthread_mutex_lock(&mutex_asociadas);
	int size_mem = list_size(mem_asociadas);

	t_tipoSeeds *memory;

	if(size_mem == 0){
		memory=malloc(sizeof(t_tipoSeeds));
		memset(memory, 0x00, sizeof(t_tipoSeeds));
		memory->numeroMemoria = -1;
		pthread_mutex_unlock(&mutex_asociadas);
		return memory;
	}

	n = rand() % size_mem;
	memory = list_get(mem_asociadas, n);
	pthread_mutex_unlock(&mutex_asociadas);

	return memory;
}

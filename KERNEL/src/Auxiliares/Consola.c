/*
 * Consola.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"

void *crear_consola() {

	char * line;
	rl_attempted_completion_function = character_name_completion;

	int estado;

	//TODO: TIEMPO QUE TARDA EN HACER GOSSIPING PARA TENER AL MENOS 1 MEMORIA CONECTADA
	//int tiempo = 20000;
	//sleep(tiempo / 1000);

	while (1) {
		line = readline("Ingrese un comando > ");

		if (!string_is_empty(line)) {
			add_history(line);
			estado = procesar_comando(line);
		}

		if (estado != 0) {
			free(line);
			return (void*)EXIT_FAILURE;
		}
	}
}

int procesar_comando(char *line) {

	log_info(logger, "CONSOLA| %s.", line);

	char* linea_auxiliar = string_new();
	string_append(&linea_auxiliar, line);
	t_request* request = parsear(line, logger);
	log_error(logger, "CONSOLA| REQUEST: %d.", request->request);
	if (request->request == -1) {

		log_error(logger, "CONSOLA| Comando desconocido: %s.", line);
		printf("Comando desconocido: %s.\n", line);

	} else if (request->es_valido == -1) {

		log_error(logger, "CONSOLA| Parámetros incompletos: %s.\n", line);
		printf("Parámetros incompletos: %s.\n", line);

	} else {

		switch (request->request) {

			case _salir:

				free(linea_auxiliar);
				free(request);
				return -1;

			case _select:

				crear_proceso(linea_auxiliar, request);

				break;

			case _insert:

				crear_proceso(linea_auxiliar, request);

//				printf("Validando existencia de Tabla.\n");
//				printf("Seleccionando Memoria según Criterio.\n");
//				printf("Enviando INSERT a una Memoria.\n");

				break;

			case _create:

				crear_proceso(linea_auxiliar, request);

				break;

			case _describe:

				crear_proceso(linea_auxiliar, request);
				// elegir memoria
				//enviarMensaje(kernel, describe, sizeof(request), &request, 10/* socket memoria*/, logger, mem);

				break;

			case _drop:

				crear_proceso(linea_auxiliar, request);

				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando DROP a una Memoria.\n");

				break;

			case _journal:

				log_info(logger, "CONSOLA| ENVIAR JOURNAL");
				enviar_journal_memorias();

				break;

			case _add:;

				int numeroMemoria = atoi(request->parametro2);
				int resultado = agregar_memoria_a_criterio(numeroMemoria, request->parametro4);
				log_info(logger, "CONSOLA| Resultado: %d", resultado);
				//Si dio ok agrego a la lista global
				if(resultado == 1){
					t_tipoSeeds* memoria = obtener_memoria_lista(numeroMemoria);
					t_tipoSeeds* mem = malloc(sizeof(t_tipoSeeds));
					memcpy(mem, memoria, sizeof(t_tipoSeeds));
					pthread_mutex_lock(&mutex_asociadas);
					list_add(mem_asociadas, mem);
					pthread_mutex_unlock(&mutex_asociadas);
				}


				break;

			case _run:;

				char* contenido = abrir_archivo_LQL(request);
				crear_proceso(contenido, request);

				free(contenido);
				break;

			case _metrics:

//				log_info(logger, "Lista NEW: %d elementos.", list_size(lista_new));
//				log_info(logger, "Lista READY: %d elementos.", list_size(lista_ready));
//				log_info(logger, "Lista EXEC: %d elementos.", list_size(lista_exec));
//				log_info(logger, "Lista EXIT: %d elementos.", list_size(lista_exit));
				get_metricas_consola();

				break;

			default:;
				// No entra por acá porque se valida antes el enum != -1
				//printf("No se reconoce el comando %s .\n", comando);
		}

	}

	if(request->parametro1) free(request->parametro1);
	if(request->parametro2) free(request->parametro2);
	if(request->parametro3) free(request->parametro3);
	if(request->parametro4) free(request->parametro4);
	if(request) free(request);
	free(linea_auxiliar);
	return 0;
}

char* abrir_archivo_LQL(t_request* request) {

	FILE *file;
	char* linea = string_new();
	char* buffer = string_new();
	size_t len = 0;
	int cantidad_lineas = 0;

	file = fopen(request->parametro1, "r");

	if (file == NULL) {
		log_error(logger, "CONSOLA|No se puede abrir el archivo: %s", request->parametro1);
		printf("No se puede abrir el archivo: %s", request->parametro1);
	}

	while (getline(&linea, &len, file) != -1) {
		printf("Contenido de línea: %s",linea);
		string_append(&buffer, linea);
		cantidad_lineas++;
	}

	printf("Cantidad de líneas: %d \n",cantidad_lineas);

	fclose(file);
	if(linea)
		free(linea);

	return buffer;
}


char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "SELECT"
			, "INSERT"
			, "CREATE"
			, "DESCRIBE"
			, "DROP"
			, "JOURNAL"
			, "ADD"
			, "RUN"
			, "METRICS"
			, "SALIR"
			, NULL };

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = character_names[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

void imprimir_pcb(t_pcb* pcb) {

	log_info(logger, "PCB| Proceso N°: %d.", pcb->id_proceso);
	log_info(logger, "PCB| Ruta archivo: %s.", pcb->ruta_archivo);
	log_info(logger, "PCB| Program Counter: %d.", pcb->program_counter);
	log_info(logger, "PCB| Cantidad request: %d.", pcb->cantidad_request);
	log_info(logger, "PCB| Script: %s", pcb->script);

}


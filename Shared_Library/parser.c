/*
 * parser.c
 *
 *  Created on: 14 may. 2019
 *      Author: utnso
 */


#include "parser.h"

char* request_str[] = {
		"SELECT"
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

t_request* parsear(char* linea, t_log* logger) {

	t_request* request = malloc(sizeof(t_request));
	char* parametro_aux = string_new();

	// Inicializo los strings
	request->parametro1 = string_new();
	request->parametro2 = string_new();
	request->parametro3 = string_new();
	request->parametro4 = string_new();
	request->es_valido = -1;

	// 1. CHEQUEAR PORQUE CUANDO RECIBO PARAMETRO ENTRE "" NO LO TOMA BIEN -----------------------
	// 2. CHEQUEAR LOS PARAMETROS NECESARIOS PARA QUE NO FALLE CON SEG FAULT ---------------------
	char* comando = strtok(linea, " ");

	request->request = string_to_enum(comando);

	if (request->request == -1) {

		log_error(logger, "PARSER|Comando desconocido: %s.", comando);

	} else {

		log_info(logger, "PARSER|Comando: %s.", comando);

		switch (request->request) {

				case _salir:
					// No hacer nada
					request->es_valido = 0;
					break;

				case _select:
					// SELECT [NOMBRE_TABLA] [KEY]
					// SELECT TABLA1 3

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					// Validación Parámetro 2
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 2 vacío.");
						break;
					}

					string_append(&request->parametro2, parametro_aux);

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					log_info(logger, "PARSER|Parámetro 2: %s", request->parametro2);
					request->es_valido = 0;
					break;

				case _insert:
					// INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
					// INSERT TABLA1 3 “Mi nombre es Lissandra”

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					// Validación Parámetro 2
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 2 vacío.");
						break;
					}

					string_append(&request->parametro2, parametro_aux);

					// Validación Parámetro 3
					parametro_aux = strtok(NULL, "\"\"");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 3 vacio.");
						break;
					}

					string_append(&request->parametro3, parametro_aux);

					// Validacion Parametro 4
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux != NULL) {
						string_append(&request->parametro4, parametro_aux);
					}

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					log_info(logger, "PARSER|Parámetro 2: %s", request->parametro2);
					log_info(logger, "PARSER|Parámetro 3: %s", request->parametro3);
					log_info(logger, "PARSER|Parámetro 4: %s", request->parametro4);
					request->es_valido = 0;
					break;

				case _create:
					// CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
					// CREATE TABLA1 SC 4 60000

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					// Validación Parámetro 2
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 2 vacío.");
						break;
					}

					string_append(&request->parametro2, parametro_aux);

					// Validación Parámetro 3
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 3 vacío.");
						break;
					}

					string_append(&request->parametro3, parametro_aux);

					// Validación Parámetro 4
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 4 vacío.");
						break;
					}

					string_append(&request->parametro4, parametro_aux);

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					log_info(logger, "PARSER|Parámetro 2: %s", request->parametro2);
					log_info(logger, "PARSER|Parámetro 3: %s", request->parametro3);
					log_info(logger, "PARSER|Parámetro 4: %s", request->parametro4);
					request->es_valido = 0;
					break;

				case _describe:
					// DESCRIBE [NOMBRE_TABLA]
					// DESCRIBE
					// DESCRIBE TABLA1
					parametro_aux = strtok(NULL, " ");
					log_info(logger, "PARSER|REQUEST: %d", request->request);
					if(parametro_aux != NULL) {
						string_append(&request->parametro1, parametro_aux);
						log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					}

					request->es_valido = 0;
					break;

				case _drop:
					// DROP [NOMBRE_TABLA]
					// DROP TABLA1

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					request->es_valido = 0;
					break;

				case _journal:
					// No tiene parámetros
					request->es_valido = 0;
					break;

				case _add:
					// ADD MEMORY [NÚMERO] TO [CRITERIO]
					// ADD MEMORY 3 TO SC

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					// Validación Parámetro 2
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 2 vacío.");
						break;
					}

					string_append(&request->parametro2, parametro_aux);

					// Validación Parámetro 3
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 3 vacío.");
						break;
					}

					string_append(&request->parametro3, parametro_aux);

					// Validación Parámetro 4
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 4 vacío.");
						break;
					}

					string_append(&request->parametro4, parametro_aux);

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					log_info(logger, "PARSER|Parámetro 2: %s", request->parametro2);
					log_info(logger, "PARSER|Parámetro 3: %s", request->parametro3);
					log_info(logger, "PARSER|Parámetro 4: %s", request->parametro4);
					request->es_valido = 0;
					break;

				case _run:
					// RUN [PATH]
					// RUN ../SCRIPTS/SCRIPT.LQL

					// Validación Parámetro 1
					parametro_aux = strtok(NULL, " ");

					if(parametro_aux == NULL) {
						log_error(logger, "PARSER|Parámetro 1 vacío.");
						break;
					}

					string_append(&request->parametro1, parametro_aux);

					log_info(logger, "PARSER|Parámetro 1: %s", request->parametro1);
					request->es_valido = 0;
					break;

				case _metrics:
					// No tiene parámetros

					request->es_valido = 0;
					break;

				default:
					// No entra por acá porque se valida antes el enum != -1
					break;
			}

	}

//	free(parametro_aux);
	return request;

}


// Auxiliares
t_request_enum string_to_enum(char *sval) {
	t_request_enum result = _select;
	int i = 0;
	for (i = 0; request_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, request_str[i]))
			return result;
	return -1;
}

char* enum_to_string(t_request_enum comando) {
	return request_str[comando];
}

/*
 * parser.h
 *
 *  Created on: 14 may. 2019
 *      Author: utnso
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "shared.h"
//#include <stdlib.h>
//#include <commons/string.h>
//#include "Logueo.h"

typedef enum t_request_enum {
	_select
	, _insert
	, _create
	, _describe
	, _drop
	, _journal
	, _add
	, _run
	, _metrics
	, _salir
} t_request_enum;

typedef struct {

	t_request_enum request;
	char* parametro1;
	char* parametro2;
	char* parametro3;
	char* parametro4;
	int es_valido;
} t_request;


t_request* parsear(char* linea, t_log* parser);
t_request_enum string_to_enum(char*);
char* enum_to_string(t_request_enum);


#endif /* PARSER_H_ */

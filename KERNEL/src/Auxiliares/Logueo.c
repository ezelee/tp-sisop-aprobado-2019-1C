/*
 * Logueo.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */


#include "Logueo.h"

void loguear_string(char* string) {

	log_info(logger, string);
}

void loguear_carga_propiedad(char* propiedad, char* valor) {
	char* str = string_new();
	char* descripcion_prop = string_new();
	char* descripcion_valor = string_new();
	string_append(&descripcion_prop,"CONFIG| Propiedad: %s. ");
	string_append(&descripcion_valor,"Valor: %s.");
	string_append_with_format(&str, descripcion_prop, propiedad);
	string_append_with_format(&str, descripcion_valor, valor);
	log_info(logger, str);
	free(valor);
	free(descripcion_valor);
	free(descripcion_prop);
	free(str);
}

void loguear_error_carga_propiedad(char* propiedad) {
	char* str = string_new();
	char* descripcion = string_new();
	string_append(&descripcion,"CONFIG|Error carga propiedad: %s");
	string_append_with_format(&str, descripcion, propiedad);
	log_error(logger, str);
	free(descripcion);
	free(str);
}

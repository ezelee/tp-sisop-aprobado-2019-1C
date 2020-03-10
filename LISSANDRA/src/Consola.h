/*
 * Consola.h
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "LFS.h"
#include <readline/readline.h>
#include <readline/history.h>

typedef enum t_tipoComando {
	select_,
	insert_,
	create_,
	describe_,
	drop_,
	salir_
} t_tipoComando;

void *crear_consola();
int procesar_comando(char *line);
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
t_tipoComando buscar_enum_lfs(char *sval);
void liberar_comando(char ** comando);



#endif /* CONSOLA_H_ */

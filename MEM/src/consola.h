/*
 * consola.h
 *
 *  Created on: 20 oct. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>

typedef enum t_tipoComando {
	select_,
	insert_,
	create_,
	describe_,
	drop_,
	journal_,
	dump_,
	salir_
} t_tipoComando;

pthread_t consola;

void crearHiloConsola();

#endif /* CONSOLA_H_ */

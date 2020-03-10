/*
 * error.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

void exit_gracefully(int return_nr);
void _exit_with_error(char* error_msg, void * buffer);

#endif /* ERROR_H_ */

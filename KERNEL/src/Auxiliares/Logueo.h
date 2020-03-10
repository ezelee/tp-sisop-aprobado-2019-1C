/*
 * Logueo.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_LOGUEO_H_
#define AUXILIARES_LOGUEO_H_

#include <shared.h>

// Variables globales
t_log *logger;


// Definición de funciones
void loguear_string(char*);
void loguear_carga_propiedad(char*, char*);
void loguear_error_carga_propiedad(char* );

#endif /* AUXILIARES_LOGUEO_H_ */

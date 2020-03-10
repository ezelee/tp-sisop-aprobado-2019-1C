/*
 * Conexion.h
 *
 *  Created on: 22 abr. 2019
 *      Author: utnso
 */

#include "../KERNEL.h"
#include <shared.h>
#include "Logueo.h"

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_


int conectar_a_servidor(char* ip, int puerto, int proceso);
void enviar_journal_memorias();
void enviar_journal_sc();
void enviar_journal_shc();
void enviar_journal_ev();
void enviar_mensaje_journal(t_tipoSeeds *memoria);
int conectar_a_memoria(t_tipoSeeds* memoria);
void remover_memoria(t_tipoSeeds* memoria);
void remove_memoria_hashdictionary(int numeroMemoria);


#endif /* AUXILIARES_CONEXION_H_ */

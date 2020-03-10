/*
 * Dump.h
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */

#ifndef DUMP_H_
#define DUMP_H_

#include "LFS.h"

int dumpRealizados;

void *InicializarDump();
void AplicarTiempoDump();
void RealizarDumpeo();
void LimpiarMemtable();
void DumpearTabla(t_list *lista, char *nombre);
void AumentarContadorTmp(char *nombre);
void ReiniciarContadorTmp(char *nombre);
int GetContadorTmp(char *nombre);
int GetEstadoTabla(char *nombre);
void BloquearTabla(char *nombre);

#endif /* DUMP_H_ */

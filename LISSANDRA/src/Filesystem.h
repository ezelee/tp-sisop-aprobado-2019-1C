/*
 * Filesystem.h
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "LFS.h"

//structs
typedef struct {
	char tipoConsistencia[10];
	int particiones;
	int compactationTime;
} t_metadata;



//variables globales
t_config *config_metadata;
t_bitarray *bitmap;

int tamanio_bloques;
int cantidad_bloques;
char *rutaMetadata;
char *rutaBitmap;
char *rutaTablas;
char *rutaBloques;
char *bmap;


//funciones filesystem
void *CrearFileSystem();
void CargarMetadata();
void CargarBitmap();
void CargarTablas();
int ExisteTabla(const char*tabla);
int CalcularParticion(int clave, int particiones);
void CrearDirectorioTabla(char *tabla);
void CrearMetadataTabla(char *tabla, char *consistencia, int particiones, int tiempoCompactacion);
int InsertarTabla(t_insert *insert);
void CrearBloque(int numero, int bytes);
int ContarElementosArray(char **cadena);
t_registro* BuscarKeyParticion(int key, char *bloque);
t_registro* BuscarKey(t_select *selectMsg);
int AgregarBloque();
int CrearTabla(t_create *msgCreate);
void ObtenerMetadataCompleto();
int DropearTabla(char *nombre);
int CalcularBloques(int bytes);
void LiberarBloques(char **bloques, int cantBloques);
void LiberarMetadata(char **bloques, int cant);
t_list *BuscarKeyMemtable(int key, char *nombre);
void LevantarHilosCompactacionFS();
/**Invocar con extension ".bin" o ".tmp" **/
t_list *ObtenerRegistros(char *tabla, char *extension);
t_list *ObtenerRegistrosArchivo(char *tabla, char *archivo, char *extension);
void GuardarEnBloque(char *linea, char *path);
int ContarTablas();
t_list *BuscarKeyTemporales(int key, char *tabla);
void AddGlobalList(char *nombre);
int GetFreeBlocks();
void IniciarBloques();
void CrearDirectorio(char *directory);
void aplicar_retardo();
void RemoveGlobalList(char *tabla);
char* descomponer_registro(t_registro *registro);
char* descomponer_metadata(t_metadata *metadata, char *nombre);
t_metadata* ObtenerMetadataTabla(char *tabla);


#endif /* FILESYSTEM_H_ */

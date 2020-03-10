/*
 ============================================================================
 Name        : LISSANDRA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "LFS.h"

int main(void) {

	logger = configurar_logger("../log/LIS.log", "Lissandra");
	CargarConfigLFS();
	Inicializar();
	CrearFileSystem();
	//Creacion de hilos
	CrearHiloConsola();
	CrearHiloConexiones();
	CrearHiloDump();
	LevantarHilosCompactacionFS();
	CrearHiloInotify();
	pthread_join(thread_consola, NULL);
	return EXIT_SUCCESS;

}

void Inicializar()
{
	memtable = list_create();
	tablasGlobal = list_create();
	listaHilos = list_create();

	pthread_mutex_init(&mutex_temp, NULL);
	pthread_mutex_init(&mutex_compactacion, NULL);
	pthread_mutex_init(&mutex_bitmap, NULL);
}

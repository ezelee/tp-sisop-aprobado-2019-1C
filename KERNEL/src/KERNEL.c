/*
 ============================================================================
 Name        : KERNEL.c
 Author      : Osquic
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "KERNEL.h"

int main() {

	/* 1. Logger */
	logger = configurar_logger("../log/KERNEL.log", "Kernel");
//	logger = configurar_logger("/home/utnso/Repositorios/tp-2019-1c-One-more-time//KERNEL/log/KERNEL.log", "Kernel");

	/* 2. Configuración */
	cargar_configuracion_kernel("../config/KERNEL.config");
//	cargar_configuracion_kernel("/home/utnso/Repositorios/tp-2019-1c-One-more-time//KERNEL/config/KERNEL.config");

	/* 3. Inicializar variables */
	inicializar();

	/* 4. Consola */
	crear_hilo_consola();

	/* 5. Inotify */
	crear_hilo_inotify();

	/* 6. Gossiping */
	init_gossiping();
	crear_hilo_gossiping();

	/* 7. Refresh metadata */
	crear_hilo_refresh();

	/* 8. Planificador */
	crear_hilo_planificador();

	/* 9. Metricas */
	crear_hilo_metricas();

	//planificar();

	pthread_join(thread_consola, NULL);

	puts("Muchas gracias por utilizar el proceso KERNEL. Vuelva pronto!");
	return EXIT_SUCCESS;
}

void inicializar() {

	cont_id_procesos = 0;
	hashdictionary = dictionary_create();

	inicializar_listas();
	inicializar_semaforos();
}

void inicializar_semaforos() {

	sem_init(&sem_new, 0, 0);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_exit, 0, 0);

	sem_init(&sem_multiprog, 0, kernel_conf.multiprocesamiento);
	pthread_mutex_init(&mutex_metadata, NULL);
	pthread_mutex_init(&mutex_memoria_sc, NULL);
	pthread_mutex_init(&mutex_memoria_shc, NULL);
	pthread_mutex_init(&mutex_memoria_ev, NULL);

	pthread_mutex_init(&mutex_LISTA_CONN, NULL);

	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_asociadas, NULL);
}

void inicializar_listas() {

	lista_new = list_create();
	lista_ready = list_create();
	lista_exec = list_create();
	lista_exit = list_create();
	lista_criterio_shc = list_create();
	lista_criterio_ev = list_create();
	lista_metadata = list_create();
	LISTA_CONN = list_create();
	mem_asociadas = list_create();
}


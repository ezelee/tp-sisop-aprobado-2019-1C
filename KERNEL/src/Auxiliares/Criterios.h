/*
 * Criterios.h
 *
 *  Created on: 22 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CRITERIOS_H_
#define AUXILIARES_CRITERIOS_H_

#include "../KERNEL.h"

//t_tipoSeeds* memoria_sc;
t_dictionary *hashdictionary;
typedef enum tipoCriterio {
	SC,SHC,EC
} t_tipoCriterio;


t_tipoCriterio criterio_to_enum(char *sval);
t_tipoSeeds* obtener_memoria_lista_SHC(int numero);
t_tipoSeeds* obtener_memoria_lista(int numero);
int agregar_memoria_a_criterio(int nro_memoria, char* criterio);
int enviarjournalSegunCriterio(char *criterio);
t_tipoSeeds* get_memoria_por_criterio(char *criterio, int key);
int existe_memoria(t_tipoSeeds* memoria, t_list* lista);
int get_memory_hash_SHC(int key);
t_tipoSeeds * get_memoria_asociada();

#endif /* AUXILIARES_CRITERIOS_H_ */

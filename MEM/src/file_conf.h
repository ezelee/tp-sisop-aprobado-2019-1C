#ifndef FILE_CONF_H_
#define FILE_CONF_H_

#include <shared.h>

#include <stdio.h>
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdint.h>

t_config * config;

typedef struct  {
  char *PUERTO;
  char *IP_FS;
  char *PUERTO_FS;
  char **IP_SEEDS;
  char **PUERTO_SEEDS;
  char **MEMORY_NUMBER_SEEDS;
  uint32_t RETARDO_MEM;
  uint32_t RETARDO_FS;
  uint32_t TAM_MEM;
  uint32_t RETARDO_JOURNAL;
  uint32_t RETARDO_GOSSIPING;
  uint32_t MEMORY_NUMBER;
  char *IP;
} t_MEM_CONF;

t_MEM_CONF MEM_CONF;

void get_config(char* path);
void get_modify_config(char* path);

#endif /* FILE_CONF_H_ */

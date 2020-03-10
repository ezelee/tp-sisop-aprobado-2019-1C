/*
 * journaling.h
 *
 *  Created on: 12 may. 2019
 *      Author: utnso
 */

#ifndef JOURNALING_H_
#define JOURNALING_H_

#include <signal.h>
#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>

int process_journaling();

int crearHiloJournaling();

pthread_t journaling;
pthread_mutex_t journalingMutex;
//pthread_mutex_t journalingMutexSelect;
//pthread_mutex_t journalingMutexInsert;
//pthread_mutex_t journalingMutexCreate;
//pthread_mutex_t journalingMutexDescribe;
//pthread_mutex_t journalingMutexDrop;

#endif /* JOURNALING_H_ */

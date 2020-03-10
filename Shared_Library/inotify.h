/*
 * inotify.h
 *
 *  Created on: 13 jun. 2019
 *      Author: utnso
 */

#ifndef INOTIFY_H_
#define INOTIFY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include <limits.h>
#include <signal.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

pthread_t inotify;

int inotifyInit(char* args_configfile);
int inotifyEvent(int inotifyFd);

#endif /* INOTIFY_H_ */

/*
 * inotify.c
 *
 *  Created on: 13 jun. 2019
 *      Author: utnso
 */

#include "inotify.h"

#include "commons/string.h"

int inotifyInit(char* args_configfile)
{
	int inotifyFd, wd;

	//if (argc < 2 || strcmp(argv[1], "--help") == 0)
		//usageErr("%s pathname...\n", argv[0]);

	inotifyFd = inotify_init();/* Create inotify instance */
	if (inotifyFd == -1)
		return -1;

	wd = inotify_add_watch(inotifyFd, args_configfile, IN_ALL_EVENTS);
	if (wd == -1)
		return -1;

	return inotifyFd;
}
int displayInotifyEvent(struct inotify_event *i)
{
	//	printf("    wd =%2d; ", i->wd);
	//	if (i->cookie > 0)
	//		printf("cookie =%4d; ", i->cookie);
	//	printf("mask = ");
	//	if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
	//	if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
	//	if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
	//	if (i->mask & IN_MODIFY)   		printf("IN_MODIFY ");
	//	if (i->mask & IN_CREATE)        printf("IN_CREATE ");
	//	if (i->mask & IN_DELETE)        printf("IN_DELETE ");
	//	if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
	//	if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
	//	if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
	//	if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
	//	if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
	//	if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
	//	if (i->mask & IN_OPEN)          printf("IN_OPEN ");
	//	if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
	//	if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
	if (i->mask & IN_CLOSE_WRITE){
		//printf("IN_CLOSE_WRITE ");
		return 1;
	}
	//	printf("\n");
	//
	//	if (i->len > 0)
	//		printf("        name = %s\n", i->name);
	return 0;
}

int inotifyEvent(int inotifyFd)
{
	ssize_t numRead;
	char buf[BUF_LEN] __attribute__ ((aligned(8)));
	struct inotify_event *event;
	char *p;

	numRead = read(inotifyFd, buf, BUF_LEN);
	if (numRead == 0)
		return 0;

	if (numRead == -1)
		return 0;

	for (p = buf; p < buf + numRead; ) {
		event = (struct inotify_event *) p;
		if(displayInotifyEvent(event))
			return 1;

		p += sizeof(struct inotify_event) + event->len;
	}

	return 0;
}

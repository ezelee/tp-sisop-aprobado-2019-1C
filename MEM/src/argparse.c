/*
 * argparse.c
 *
 *  Created on: 28 oct. 2018
 *      Author: utnso
 */
#include "MEM.h"

#include "argparse.h"

void destroy_args() {
	free(args_configfile);
	free(args_verbose);
}
void initArgumentos(int argc, char** argv) {
	int opt;
	args_configfile = string_from_format("false");
	args_verbose = string_from_format("false");
	while((opt = getopt(argc, argv, ":vf:")) != -1)
	{
		switch(opt)
		{
			case 'v':
				printf("option: %c\n", opt);
				free(args_verbose);
				args_verbose = string_from_format("true");
				break;
			case 'f':
				printf("filename: %s\n", optarg);
				free(args_configfile);
				args_configfile = string_from_format("%s",optarg);
				break;
			case ':':
				printf("option needs a value\n");
				break;
		}
	}
}

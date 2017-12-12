#ifndef _ARG_H
#define _ARG_H

#include "ressource.h"

typedef struct{
	char *input;
	char *metadata;
	unsigned int trigger;
	unsigned int data_offset;
}Arg;

Arg arg_ProgramInput(int argc, char *argv[]);

#endif

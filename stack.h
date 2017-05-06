#ifndef STACK_H_
#define STACK_H_
#include <stdio.h>

typedef struct localStack_t{ 
	char local[100][100];
	int top;
}localStack_t;

typedef struct globalStack_t{
	char global[100][100];
	int top;	
}globalStack_t;

#endif

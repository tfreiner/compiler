#ifndef NODE_H_
#define NODE_H_
#include <stdio.h>
#include "token.h"

//----the node structure----
//Contains a label, tokens, and node pointers which are the children.

typedef struct node_t{
	char* label;
	int level;
	token_t *token1;
	token_t *token2;		
	struct node_t* child1;
	struct node_t* child2;
	struct node_t* child3;
	struct node_t* child4;	
}node_t;

#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "node.h"
#include "stack.h"

static void traverse(stack_t*, node_t*, int);
static void push(stack_t*, char*);
static void pop(stack_t*);
static int find(stack_t*, char*, int);
static void error(int, token_t*);
static int varCount = 0;

void sem(node_t* node){
	stack_t stack;
	stack.top = -1;	
	traverse(&stack, node, 0);
}

static void push(stack_t* stack, char* token){
	stack->stack[stack->top] = token;
	stack->top++;
}

static void pop(stack_t* stack){
	stack->top--;
}

static int find(stack_t* stack, char* token, int level){
	for(int i = 0; i < level; i++){
		if(strcmp(stack->stack[i], token))
			return i;
	}
	return -1;		
}

static void traverse(stack_t* stack, node_t* node, int level){
	int tokenLevel = 0;
	if(node == NULL)
		return;
	if(strcmp(node->label, "program") == 0){
		traverse(stack, node->child1, level+1);
		traverse(stack, node->child2, level+1);		
	}
	else if(strcmp(node->label, "vars") == 0){
		if(varCount == 0){
			tokenLevel = find(stack, node->token1->tokenInstance, level);
			if(tokenLevel == -1){
				push(stack, node->token1->tokenInstance);	
				varCount++;
			}
			else
				error(1, node->token1);
		}
		else if(varCount > 0){
			varCount++;			
		}
		traverse(stack, node->child1, level+1);
	}
	else if(strcmp(node->label, "block") == 0){
		varCount = 0;

		traverse(stack, node->child1, level+1);
		traverse(stack, node->child2, level+1);

		for(int i = 0; i < varCount; i++){
			pop(stack);
		}
	
	}
	else if(strcmp(node->label, "mvars") == 0){
		tokenLevel = find(stack, node->token1->tokenInstance, level);
		if(tokenLevel == -1){
			push(stack, node->token1->tokenInstance);
		}
		else
			error(1, node->token2);
	}
}

static void error(int errorNum, token_t* token){}

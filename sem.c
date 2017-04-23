#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "node.h"
#include "stack.h"

static void traverse(localStack_t*, globalStack_t*, node_t*);
static void push(localStack_t*, char*);
static void pop(localStack_t*);
static int find(localStack_t*, char*);
static void insert(globalStack_t*, char*);
static bool verify(globalStack_t*, char* token);
static void error(int, token_t*);
static int varCount = 0;
static int outerVar = 0;

void sem(node_t* node){
	localStack_t stack;
	stack.top = -1;
	globalStack_t global;
	global.top = -1;	
	traverse(&stack, &global, node);
}

static void push(localStack_t* stack, char* token){
	stack->top++;
	strcpy(stack->localStack[stack->top], token);
}

static void pop(localStack_t* stack){
	strcpy(stack->localStack[stack->top], "");
	stack->top--;
}

static int find(localStack_t* stack, char* token){
	for(int i = 0; i <= stack->top; i++){
		if(strcmp(stack->localStack[i], token) == 0){
			return i;
		}
	}
	return -1;		
}

static void insert(globalStack_t* global, char* token){
	strcpy(global->globalStack[global->top], token);
	global->top++;
}

static bool verify(globalStack_t* global, char* token){
	for(int i = 0; i <= global->top; i++){
		if(strcmp(global->globalStack[i], token) == 0)
			return false;
	}
	return true;
}

static void traverse(localStack_t* stack, globalStack_t* global, node_t* node){
	int tokenLevel = 0;
	if(node == NULL)
		return;
	if(strcmp(node->label, "program") == 0){
		printf("PROGRAM\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);		
	}
	else if(strcmp(node->label, "block") == 0){
		printf("BLOCK\n");
		varCount = 0;
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
		for(int i = 0; i < varCount; i++){
			pop(stack);
		}
	}
	else if(strcmp(node->label, "vars") == 0){
		printf("VARS\n");
		if(outerVar == 0){
			if(verify(global, node->token1->tokenInstance)){
				insert(global, node->token1->tokenInstance);
				traverse(stack, global, node->child1);
				outerVar = 1;
			}
			else
				error(1, node->token1);
		}
		else{
			int stackLevel = find(stack, node->token1->tokenInstance);
			if(stackLevel < varCount && stackLevel != -1){
				error(1, node->token1);
			}
			else{
				push(stack, node->token1->tokenInstance);
				varCount++;
				traverse(stack, global, node->child1);
			}
		}
	}
	else if(strcmp(node->label, "mvars") == 0){
		printf("MVARS\n");
		if(outerVar == 0){
			if(verify(global, node->token1->tokenInstance)){
				insert(global, node->token1->tokenInstance);
				traverse(stack, global, node->child1);
			}
			else{
				error(1, node->token1);
			}
		}
		else{
			tokenLevel = find(stack, node->token1->tokenInstance);
			if(tokenLevel < varCount && tokenLevel != -1){
				error(1, node->token1);		
			}
			else{
				push(stack, node->token1->tokenInstance);
				varCount++;
				traverse(stack, global, node->child1);
			}
		}
	}
	else if(strcmp(node->label, "expr") == 0){
		printf("MVARS\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
	}
	else if(strcmp(node->label, "M") == 0){
		printf("M\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
	}
	else if(strcmp(node->label, "T") == 0){
		printf("T\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
	}
	else if(strcmp(node->label, "F") == 0){
		printf("F\n");
		 traverse(stack, global, node->child1);
	}
	else if(strcmp(node->label, "R") == 0){
		printf("R\n");
		if(node->child1 == NULL){
			if(node->token1->tokenID == ID_tk){
				tokenLevel = find(stack, node->token1->tokenInstance);
				if(tokenLevel == -1){
					printf("NOT FOUND\n");
					if(verify(global, node->token1->tokenInstance))
						error(2, node->token1);
				}	
			}
			else{
				//need this for code gen...
			}
		}
		else{
			traverse(stack, global, node->child1);
		}
	}
	else if(strcmp(node->label, "stats") == 0){
		printf("STATS\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
	}
	else if(strcmp(node->label, "mStat") == 0){
		printf("MSTAT\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
	}
	else if(strcmp(node->label, "stat") == 0){
		printf("STAT\n");
		traverse(stack, global, node->child1);
	}
	else if(strcmp(node->label, "in") == 0){
		printf("IN\n");
		tokenLevel = find(stack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			printf("NOT FOUND\n");
			if(verify(global, node->token1->tokenInstance))
				error(2, node->token1);
		}
	}
	else if(strcmp(node->label, "out") == 0){
		printf("OUT\n");
		traverse(stack, global, node->child1);
	}
	else if(strcmp(node->label, "if") == 0){
		printf("IF\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
		traverse(stack, global, node->child3);
		traverse(stack, global, node->child4);
	}
	else if(strcmp(node->label, "loop") == 0){
		printf("LOOP\n");
		traverse(stack, global, node->child1);
		traverse(stack, global, node->child2);
		traverse(stack, global, node->child3);
		traverse(stack, global, node->child4);
	}
	else if(strcmp(node->label, "assign") == 0){
		printf("ASSIGN\n");
		tokenLevel = find(stack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			printf("NOT FOUND\n");
			if(verify(global, node->token1->tokenInstance))
				error(2, node->token1);
		}
		traverse(stack, global, node->child1);
	}
	else if(strcmp(node->label, "RO") == 0){
		printf("RO\n");
	}
	
}

static void error(int errorNumber, token_t* token){

	switch (errorNumber){
		case 0:
			printf("%s%s%s%d\n", "ERROR variable already defined: ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 1:
			printf("%s%s%s%d\n", "ERROR variable already defined: ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 2:
			printf("%s%s%s%d\n", "ERROR variable not defined: ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
	}
}

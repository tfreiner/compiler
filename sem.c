#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "stack.h"

//SEMANTIC AND CODE GEN FUNCTIONS

//static global variable declarations and function prototypes

static int labelCount = 0;
static char name[20];
static char *newName(int);
static void traverse(localStack_t*, globalStack_t*, node_t*, FILE*);
static void push(localStack_t*, char*);
static void pop(localStack_t*);
static int find(localStack_t*, char*);
static void insert(globalStack_t*, char*);
static bool verify(globalStack_t*, char* token);
static void error(int, token_t*);
static int varCount = 0;
static int outerVar = 0;
static int tempVarCount = 0;

/*
	The sem function creates the output file, initializes the stacks, and traverses the tree.
	After the traversal, it prints all the global and temporary variables to the stack after STOP.
	Then the file is closed.
*/
void sem(node_t* node, char *arg){
	strcat(arg, ".asm");
	FILE *file = fopen(arg, "w");
	if(file == NULL){
		printf("File cannot be opened, errno = %d\n", errno);
		return;
	}
	localStack_t localStack;
	localStack.top = -1;
	globalStack_t globalStack;
	globalStack.top = 0;	
	traverse(&localStack, &globalStack, node, file);
	printf("Semantic analysis successful.\n");
	printf("Code generation successful.\n");
	fprintf(file, "STOP\n");
	for(int i = 0; i < globalStack.top; i++)
		fprintf(file, "%s\t%d\n", globalStack.global[i], 0);
	for(int i = 0; i < tempVarCount; i++)
		fprintf(file, "V%d\t%d\n", i, 0);
	fclose(file);
}

/*
	The push and pop functions push and pop variables to/from the local stack, respectively.
	For push, the stack size is incremented and the token is copied to the array.
	For pop, "" is copied to the array where the token was, and the stack size is decremented.
*/
static void push(localStack_t* localStack, char* token){
	localStack->top++;
	strcpy(localStack->local[localStack->top], token);
}

static void pop(localStack_t* localStack){
	strcpy(localStack->local[localStack->top], "");
	localStack->top--;
}

/*
	The find function searches the local stack for the token and if it is found, it returns the distance from the top of the stack
	If it is not found, it returns -1.
*/
static int find(localStack_t* localStack, char* token){
	for(int i = localStack->top; i > -1; i--){
		if(strcmp(localStack->local[i], token) == 0){
			return (localStack->top - i);
		}
	}
	return -1;		
}

/*
	The insert function pushes a token to the global stack.
	The stack size is incremented and the token is copied to the stack.
*/
static void insert(globalStack_t* globalStack, char* token){
	strcpy(globalStack->global[globalStack->top], token);
	globalStack->top++;
}

/*
	The verify function searches for the token in the global stack and if it is found, it returns false, else it returns true.
*/
static bool verify(globalStack_t* globalStack, char* token){
	for(int i = 0; i <= globalStack->top; i++){
		if(strcmp(globalStack->global[i], token) == 0)
			return false;
	}
	return true;
}

/*
	The traverse function traverses through the tree, checking the current node it is at by checking the node label.
	This function generates the code to the out file, as well as checks the semantics of the source code.
	If there are any errors in the semantics, it calls the error function.
*/
static void traverse(localStack_t* localStack, globalStack_t* globalStack, node_t* node, FILE* out){
	char var[20];
	char label[20];
	char label2[20];
	int tokenLevel = 0;

	if(node == NULL)
		return;
	else if(strcmp(node->label, "program") == 0){
		traverse(localStack, globalStack, node->child1, out);
		traverse(localStack, globalStack, node->child2, out);		
	}
	else if(strcmp(node->label, "block") == 0){
		outerVar = 1;
		int blockVarCount = varCount;
		varCount = 0;
		traverse(localStack, globalStack, node->child1, out);
		traverse(localStack, globalStack, node->child2, out);
		for(int i = 0; i < varCount; i++){
			pop(localStack);
			fprintf(out, "POP\n");
		}
		varCount = blockVarCount;
	}
	else if(strcmp(node->label, "vars") == 0){
		if(outerVar == 0){
			if(verify(globalStack, node->token1->tokenInstance)){
				insert(globalStack, node->token1->tokenInstance);
				traverse(localStack, globalStack, node->child1, out);
				outerVar = 1;
			}
			else
				error(1, node->token1);
		}
		else{
			int stackLevel = find(localStack, node->token1->tokenInstance);
			if(stackLevel < varCount && stackLevel != -1){
				error(1, node->token1);
			}
			else{
				push(localStack, node->token1->tokenInstance);
				fprintf(out, "PUSH\n");
				varCount++;
				traverse(localStack, globalStack, node->child1, out);
			}
		}
	}
	else if(strcmp(node->label, "mvars") == 0){
		if(outerVar == 0){
			if(verify(globalStack, node->token1->tokenInstance)){
				insert(globalStack, node->token1->tokenInstance);
				traverse(localStack, globalStack, node->child1, out);
			}
			else{
				error(1, node->token1);
			}
		}
		else{
			tokenLevel = find(localStack, node->token1->tokenInstance);
			if(tokenLevel < varCount && tokenLevel != -1){
				error(1, node->token1);		
			}
			else{
				push(localStack, node->token1->tokenInstance);
				fprintf(out, "PUSH\n");
				varCount++;
				traverse(localStack, globalStack, node->child1, out);
			}
		}
	}
	else if(strcmp(node->label, "expr") == 0){
		if(node->token1 == NULL)
			traverse(localStack, globalStack, node->child1, out);
		else{
			traverse(localStack, globalStack, node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(localStack, globalStack, node->child1, out);
			fprintf(out, "ADD\t%s\n", var);			
		}
	}
	else if(strcmp(node->label, "M") == 0){
		if(node->token1 == NULL)
			traverse(localStack, globalStack, node->child1, out);
		else{
			traverse(localStack, globalStack, node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(localStack, globalStack, node->child1, out);
			fprintf(out, "SUB\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "T") == 0){
		if(node->token1 == NULL)
			traverse(localStack, globalStack, node->child1, out);
		else{
			traverse(localStack, globalStack, node->child2, out);
			strcpy(var, newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(localStack, globalStack, node->child1, out);
			if(strcmp(node->token1->tokenInstance, "*") == 0)
				fprintf(out, "MULT\t%s\n", var);
			else
				fprintf(out, "DIV\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "F") == 0){
		if(node->token1 == NULL)
			traverse(localStack, globalStack, node->child1, out);
		else{
			traverse(localStack, globalStack, node->child1, out);
			fprintf(out, "MULT\t%d\n", -1);
		}
	}	
	else if(strcmp(node->label, "R") == 0){
		if(node->child1 == NULL){
			if(node->token1->tokenID == ID_tk){
				tokenLevel = find(localStack, node->token1->tokenInstance);
				if(tokenLevel == -1){
					if(verify(globalStack, node->token1->tokenInstance)){
						error(2, node->token1);
					}
					fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
				}
				else if(tokenLevel > -1){
					strcpy(var, newName(1));
					fprintf(out, "LOAD\t%s\n", var);
					fprintf(out, "STACKR\t%d\n", tokenLevel);
				}
			}
			else{
				fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
			}
		}
		else{
			traverse(localStack, globalStack, node->child1, out);
		}
	}
	else if(strcmp(node->label, "stats") == 0){
		traverse(localStack, globalStack, node->child1, out);
		traverse(localStack, globalStack, node->child2, out);
	}
	else if(strcmp(node->label, "mStat") == 0){
		traverse(localStack, globalStack, node->child1, out);
		traverse(localStack, globalStack, node->child2, out);
	}
	else if(strcmp(node->label, "stat") == 0){
		traverse(localStack, globalStack, node->child1, out);
	}
	else if(strcmp(node->label, "in") == 0){
		tokenLevel = find(localStack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			if(verify(globalStack, node->token1->tokenInstance))
				error(2, node->token1);
			else
				fprintf(out, "READ\t%s\n", node->token1->tokenInstance);
		}
		else if(tokenLevel > -1){
			strcpy(var, newName(1));
			fprintf(out, "READ\t%s\n", var);
			fprintf(out, "LOAD\t%s\n", var);
			fprintf(out, "STACKW\t%d\n", tokenLevel);
		}	
	}
	else if(strcmp(node->label, "out") == 0){
		traverse(localStack, globalStack, node->child1, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		fprintf(out, "WRITE\t%s\n", var);
	}
	else if(strcmp(node->label, "if") == 0){
		traverse(localStack, globalStack, node->child3, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		traverse(localStack, globalStack, node->child1, out);
		fprintf(out, "SUB\t%s\n", var);
		strcpy(label, newName(0));
		if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2 == NULL){
			fprintf(out, "BRNEG\t%s\n", label);
			fprintf(out, "BRZERO\t%s\n", label);
		}
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2 == NULL){
			fprintf(out, "BRPOS\t%s\n", label);
			fprintf(out, "BRZERO\t%s\n", label);
		}
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRPOS\t%s\n", label);
		else if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRNEG\t%s\n", label);
		else if(node->child2->token1->tokenID == EQ_tk){
			fprintf(out, "BRPOS\t%s\n", label);
			fprintf(out, "BRNEG\t%s\n", label);
		}
		else if(node->child2->token1->tokenID == NOT_tk)
			fprintf(out, "BRZERO\t%s\n", label);
		traverse(localStack, globalStack, node->child4, out);
		fprintf(out, "\t%s:\tNOOP\n", label);
	}
	else if(strcmp(node->label, "loop") == 0){
		strcpy(label, newName(0));
		strcpy(label2, newName(0));
		fprintf(out, "\t%s:\tNOOP\n", label);
		traverse(localStack, globalStack, node->child3, out);	
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		traverse(localStack, globalStack, node->child1, out);
		fprintf(out, "SUB\t%s\n", var);
		if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2 == NULL){
			fprintf(out, "BRNEG\t%s\n", label2);
			fprintf(out, "BRZERO\t%s\n", label2);
		}
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2 == NULL){
			fprintf(out, "BRPOS\t%s\n", label2);
			fprintf(out, "BRZERO\t%s\n", label2);
		}
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRPOS\t%s\n", label2);
		else if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRNEG\t%s\n", label2);
		else if(node->child2->token1->tokenID == EQ_tk){
			fprintf(out, "BRPOS\t%s\n", label2);
			fprintf(out, "BRNEG\t%s\n", label2);
		}
		else if(node->child2->token1->tokenID == NOT_tk)
			fprintf(out, "BRZERO\t%s\n", label2);
		traverse(localStack, globalStack, node->child4, out);
		fprintf(out, "BR\t%s\n", label);
		fprintf(out, "\t%s:\tNOOP\n", label2);
	}
	else if(strcmp(node->label, "assign") == 0){
		traverse(localStack, globalStack, node->child1, out);
		tokenLevel = find(localStack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			if(verify(globalStack, node->token1->tokenInstance))
				error(2, node->token1);
			fprintf(out, "STORE\t%s\n", node->token1->tokenInstance);
		}
		else if(tokenLevel > -1){
			strcpy(var, newName(1));
			fprintf(out, "STORE\t%s\n", var);
			fprintf(out, "LOAD\t%s\n", var);
			fprintf(out, "STACKW\t%d\n", tokenLevel);

		}
	}
}

/*
	This function prints errors based on the number passed to it.
*/
static void error(int errorNumber, token_t* token){
	switch (errorNumber){
		case 1:
			printf("%s%s%s%d\n", "ERROR variable already defined: ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 2:
			printf("%s%s%s%d\n", "ERROR variable not defined: ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
	}
}

static char *newName(int choice){
	if (choice == 1)
		sprintf(name, "V%d", tempVarCount++);
	else
		sprintf(name, "L%d", labelCount++);
	return (name);
}

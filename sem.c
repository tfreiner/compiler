#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "stack.h"

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

void sem(node_t* node){
	FILE *file = fopen("project.asm", "w");
	if(file == NULL){
		printf("File cannot be opened, errno = %d\n", errno);
		return;
	}
	localStack_t stack;
	stack.top = -1;
	globalStack_t global;
	global.top = 0;	
	traverse(&stack, &global, node, file);
	printf("Semantic analysis successful.\n");
	printf("Code generation successful.\n");
	fprintf(file, "STOP\n");
	for(int i = 0; i < global.top; i++)
		fprintf(file, "%s\t%d\n", global.globalStack[i], 0);
	for(int i = 0; i < tempVarCount; i++)
		fprintf(file, "V%d\t%d\n", i, 0);
	fclose(file);
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
	for(int i = stack->top; i > -1; i--){
		if(strcmp(stack->localStack[i], token) == 0){
			printf("i: %d top of stack: %d", i, stack->top);
			printf("LOCAL STACK LOCATION for %s is : %d\n", token, (stack->top - i));
			return (stack->top - i);
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

static void traverse(localStack_t* stack, globalStack_t* global, node_t* node, FILE* out){
	char var[20];
	char label[20];
	char label2[20];
	int tokenLevel = 0;

	if(node == NULL)
		return;
	else if(strcmp(node->label, "program") == 0){
		traverse(stack, global, node->child1, out);
		traverse(stack, global, node->child2, out);		
	}
	else if(strcmp(node->label, "block") == 0){
		outerVar = 1;
		int blockVarCount = varCount;
		varCount = 0;
		traverse(stack, global, node->child1, out);
		traverse(stack, global, node->child2, out);
		for(int i = 0; i < varCount; i++){
			pop(stack);
			fprintf(out, "POP\n");
		}
		varCount = blockVarCount;
	}
	else if(strcmp(node->label, "vars") == 0){
		if(outerVar == 0){
			if(verify(global, node->token1->tokenInstance)){
				insert(global, node->token1->tokenInstance);
				//fprintf(out, "PUSH\n");//ADDEDD			
				//strcpy(var, newName(1));
				//fprintf(out, "STORE\t%s\n", var);
				traverse(stack, global, node->child1, out);
				outerVar = 1;
			}
			else
				error(1, node->token1);
		}
		else{
			printf(" here %d\n", stack->top);
			int stackLevel = find(stack, node->token1->tokenInstance);
			printf("after find %d\n", stack->top);
			if(stackLevel < varCount && stackLevel != -1){
				error(1, node->token1);
			}
			else{
				printf("PUSHING %s\n", node->token1->tokenInstance);
				push(stack, node->token1->tokenInstance);
				printf("tos %d\n", stack->top);
				fprintf(out, "PUSH\n");
				//fprintf(out, "STACKW\t%d\n", stackLevel);
				/*
				if(stackLevel == -1)
					fprintf(out, "STACKW\t%d\n", varCount);
				else
					fprintf(out, "STACKW\t%d\n", stackLevel);
				*/
				varCount++;
				traverse(stack, global, node->child1, out);
			}
		}
	}
	else if(strcmp(node->label, "mvars") == 0){
		if(outerVar == 0){
			if(verify(global, node->token1->tokenInstance)){
				insert(global, node->token1->tokenInstance);
				traverse(stack, global, node->child1, out);
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
				printf("PUSHING %s\n", node->token1->tokenInstance);
				push(stack, node->token1->tokenInstance);
				printf("tos %d\n", stack->top);
				fprintf(out, "PUSH\n");
				//fprintf(out, "STACKW\t%d\n", tokenLevel);
				varCount++;
				traverse(stack, global, node->child1, out);
			}
		}
	}
	else if(strcmp(node->label, "expr") == 0){
		if(node->token1 == NULL)
			traverse(stack, global, node->child1, out);
		else{
			traverse(stack, global, node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(stack, global, node->child1, out);
			fprintf(out, "ADD\t%s\n", var);			
		}
	}
	else if(strcmp(node->label, "M") == 0){
		if(node->token1 == NULL)
			traverse(stack, global, node->child1, out);
		else{
			traverse(stack, global, node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(stack, global, node->child1, out);
			fprintf(out, "SUB\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "T") == 0){
		if(node->token1 == NULL)
			traverse(stack, global, node->child1, out);
		else{
			traverse(stack, global, node->child2, out);
			strcpy(var, newName(1));
			fprintf(out, "STORE\t%s\n", var);
			traverse(stack, global, node->child1, out);
			if(strcmp(node->token1->tokenInstance, "*") == 0)
				fprintf(out, "MULT\t%s\n", var);
			else
				fprintf(out, "DIV\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "F") == 0){
		if(node->token1 == NULL)
			traverse(stack, global, node->child1, out);
		else{
			traverse(stack, global, node->child1, out);
			fprintf(out, "MULT\t%d\n", -1);
		}
	}	
	else if(strcmp(node->label, "R") == 0){
		if(node->child1 == NULL){
			if(node->token1->tokenID == ID_tk){
				tokenLevel = find(stack, node->token1->tokenInstance);
				if(tokenLevel == -1){
					if(verify(global, node->token1->tokenInstance)){
						error(2, node->token1);
					}
					fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
				}
				else if(tokenLevel > -1){
					strcpy(var, newName(1));
					//fprintf(out, "READ\t%s\n", var);
					fprintf(out, "LOAD\t%s\n", var);
					fprintf(out, "STACKR\t%d\n", tokenLevel);
				}
			}
			else{
				fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
			}
		}
		else{
			traverse(stack, global, node->child1, out);
		}
	}
	else if(strcmp(node->label, "stats") == 0){
		traverse(stack, global, node->child1, out);
		traverse(stack, global, node->child2, out);
	}
	else if(strcmp(node->label, "mStat") == 0){
		traverse(stack, global, node->child1, out);
		traverse(stack, global, node->child2, out);
	}
	else if(strcmp(node->label, "stat") == 0){
		traverse(stack, global, node->child1, out);
	}
	else if(strcmp(node->label, "in") == 0){
		tokenLevel = find(stack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			if(verify(global, node->token1->tokenInstance))
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
		traverse(stack, global, node->child1, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		fprintf(out, "WRITE\t%s\n", var);
	}
	else if(strcmp(node->label, "if") == 0){
		traverse(stack, global, node->child3, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		traverse(stack, global, node->child1, out);
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
		traverse(stack, global, node->child4, out);
		fprintf(out, "\t%s:\tNOOP\n", label);
	}
	else if(strcmp(node->label, "loop") == 0){
		strcpy(label, newName(0));
		strcpy(label2, newName(0));
		fprintf(out, "\t%s:\tNOOP\n", label);
		traverse(stack, global, node->child3, out);	
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		traverse(stack, global, node->child1, out);
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
		traverse(stack, global, node->child4, out);
		fprintf(out, "BR\t%s\n", label);
		fprintf(out, "\t%s:\tNOOP\n", label2);
	}
	else if(strcmp(node->label, "assign") == 0){
		traverse(stack, global, node->child1, out);
		tokenLevel = find(stack, node->token1->tokenInstance);
		if(tokenLevel == -1){
			if(verify(global, node->token1->tokenInstance))
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
	else if(strcmp(node->label, "RO") == 0){
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

static char *newName(int choice){
	if (choice == 1)
		sprintf(name, "V%d", tempVarCount++);
	else
		sprintf(name, "L%d", labelCount++);
	return (name);
}

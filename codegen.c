#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "node.h"

static int labelCount = 0;
static int varCount = 0;
static char name[20];
static char label[20];
static char *newName(int choice){
	if (choice==1)
		sprintf(name,"V%d", varCount++);
	else
		sprintf(name,"L%d", labelCount++);
	return(name);
}

static void generate(node_t*, FILE*);

void codegen(node_t *node){
	FILE *file = fopen("project.asm", "w");
	if (file == NULL){
		printf("File cannot be opened, errno = %d\n", errno);
		return;
	}
	generate(node, file);
	fprintf(file, "STOP\n");
	/*
	while(node != NULL){
		fprintf(file, "%s\tSPACE\n",node->token1->tokenInstance);
 		node=node->child1;
	}
	*/
	fprintf(file, "x\t%d\n", 0);
	for (int i = 0; i < varCount; i++) 
		fprintf(file, "V%d\t%d\n",i, 0);
	fclose(file);
}

static void generate(node_t *node, FILE *out){
	char var[20];
	if(node == NULL)
		return;
	else if(strcmp(node->label, "program") == 0){
		printf("PROGRAM\n");
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "block") == 0){
		printf("BLOCK\n");
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "vars") == 0){
		printf("VARS\n");
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "mvars") == 0){
		printf("MVARS\n");
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "expr") == 0){
		printf("EXPR\n");
		if(node->token1 == NULL)
			generate(node->child1, out);
		else{ //check
			generate(node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			generate(node->child1, out);
			fprintf(out, "ADD\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "M") == 0){
		printf("M\n");	
		if(node->token1 == NULL)
			generate(node->child1, out);
		else{ //check
			generate(node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			generate(node->child1, out);
			fprintf(out, "SUB\t%s\n", var);
		}
	}
	else if(strcmp(node->label, "T") == 0){
		printf("T\n");
		if(node->token1 == NULL)
			generate(node->child1, out);
		else{
			generate(node->child2, out);
			strcpy(var,newName(1));
			fprintf(out, "STORE\t%s\n", var);
			generate(node->child1, out);
			if(strcmp(node->token1->tokenInstance, "*") == 0)
				fprintf(out, "MULT\t%s\n", var);
			else
				fprintf(out, "DIV\t%s\n", var);
		}

	}
	else if(strcmp(node->label, "F") == 0){
		printf("F\n");
		if(node->token1 == NULL)
			generate(node->child1, out);
		else{
			generate(node->child1, out);
			fprintf(out, "MULT\t%d\n", -1);

		}
	}
	else if(strcmp(node->label, "R") == 0){
		printf("R\n");
		if(node->token1->tokenInstance == NULL)
			generate(node->child1, out);
		else{
			if(node->token1->tokenID == ID_tk)
				fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
			else
				fprintf(out, "LOAD\t%s\n", node->token1->tokenInstance);
		}
	}
	else if(strcmp(node->label, "stats") == 0){
		printf("STATS\n");
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "mStat") == 0){
		printf("MSTAT\n");
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "stat") == 0){
		printf("STAT\n");
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "in") == 0){
		printf("IN\n");
		fprintf(out, "READ\t%s\n", node->token1->tokenInstance);
	}
	else if(strcmp(node->label, "out") == 0){
		printf("OUT\n");
		generate(node->child1, out);
		strcpy(var,newName(1));
		fprintf(out, "STORE\t%s\n", var);
		fprintf(out, "WRITE\t%s\n", var);
	}
	else if(strcmp(node->label, "if") == 0){
		printf("IF\n");
		generate(node->child3, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
		fprintf(out, "SUB\t%s\n", var);
		strcpy(label, newName(0));
		if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2 == NULL)
			fprintf(out, "BRPOS\t%s\n", label);
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2 == NULL)
			fprintf(out, "BRNEG\t%s\n", label);
		else if(node->child2->token1->tokenID == LSEQLS_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRZNEG\t%s\n", label);
		else if(node->child2->token1->tokenID == GREQGR_tk && node->child2->token2->tokenID == EQ_tk)
			fprintf(out, "BRZPOS\t%s\n", label);
		else if(node->child2->token1->tokenID == EQ_tk)
			fprintf(out, "BRNZERO\t%s\n", label);
		else if(node->child2->token1->tokenID == NOT_tk)
			fprintf(out, "BRZERO\t%s\n", label);
		generate(node->child4, out);
		fprintf(out, "\t%s:\tNOOP\n", label);
	}
	else if(strcmp(node->label, "assign") == 0){
		printf("ASSIGN\n");
		generate(node->child1, out);
		fprintf(out, "STORE\t%s\n", node->token1->tokenInstance);
	}
}

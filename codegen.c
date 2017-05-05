
//ALL TEST FILES WORK EXCEPT 10, 11, AND 12.  THEY NEED LOCAL STORAGE.

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "node.h"

static int labelCount = 0;
static int varCount = 0;
static char name[20];

static char *newName(int);

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
//	fprintf(file, "y\t%d\n", 0);
//	fprintf(file, "z\t%d\n", 0);
	for (int i = 0; i < varCount; i++) 
		fprintf(file, "V%d\t%d\n",i, 0);
	fclose(file);
}

static void generate(node_t *node, FILE *out){
	char var[20];
	char label[20];
	char label2[20];
	if(node == NULL)
		return;
	else if(strcmp(node->label, "program") == 0){
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "block") == 0){
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "vars") == 0){
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "mvars") == 0){
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "expr") == 0){
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
		if(node->token1 == NULL)
			generate(node->child1, out);
		else{
			generate(node->child1, out);
			fprintf(out, "MULT\t%d\n", -1);
		}
	}
	else if(strcmp(node->label, "R") == 0){
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
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "mStat") == 0){
		generate(node->child1, out);
		generate(node->child2, out);
	}
	else if(strcmp(node->label, "stat") == 0){
		generate(node->child1, out);
	}
	else if(strcmp(node->label, "in") == 0){
		fprintf(out, "READ\t%s\n", node->token1->tokenInstance);
	}
	else if(strcmp(node->label, "out") == 0){
		generate(node->child1, out);
		strcpy(var,newName(1));
		fprintf(out, "STORE\t%s\n", var);
		fprintf(out, "WRITE\t%s\n", var);
	}
	else if(strcmp(node->label, "if") == 0){
		generate(node->child3, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
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
		generate(node->child4, out);
		fprintf(out, "\t%s:\tNOOP\n", label);
	}
	else if(strcmp(node->label, "loop") == 0){
		strcpy(label, newName(0));
		strcpy(label2, newName(0));
		fprintf(out, "\t%s:\tNOOP\n", label);
		generate(node->child3, out);
		strcpy(var, newName(1));
		fprintf(out, "STORE\t%s\n", var);
		generate(node->child1, out);
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
		generate(node->child4, out);
		fprintf(out, "BR\t%s\n", label);
		fprintf(out, "\t%s:\tNOOP\n", label2);
	}
	else if(strcmp(node->label, "assign") == 0){
		generate(node->child1, out);
		fprintf(out, "STORE\t%s\n", node->token1->tokenInstance);
	}
}

static char *newName(int choice){
	if (choice==1)
		sprintf(name,"V%d", varCount++);
	else    
		sprintf(name,"L%d", labelCount++);
	return(name);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "node.h"
#include "scanner.h"
#include "treePrint.h"
#include "sem.h"

//Function prototypes for every nonterminal in BNF
static node_t* program(token_t*, FILE*);
static node_t* block(token_t*, FILE*);
static node_t* vars(token_t*, FILE*);
static node_t* mvars(token_t*, FILE*);
static node_t* expr(token_t*, FILE*);
static node_t* M(token_t*, FILE*);
static node_t* T(token_t*, FILE*);
static node_t* F(token_t*, FILE*);
static node_t* R(token_t*, FILE*);
static node_t* stats(token_t*, FILE*);
static node_t* mStat(token_t*, FILE*);
static node_t* stat(token_t*, FILE*);
static node_t* in(token_t*, FILE*);
static node_t* out(token_t*, FILE*);
static node_t* iff(token_t*, FILE*);
static node_t* loop(token_t*, FILE*);
static node_t* assign(token_t*, FILE*);
static node_t* RO(token_t*, FILE*);
static void error(int, token_t*);
static node_t* getNode(char*);
static token_t* getToken(token_t*);

//----parser function----
//Creates the root node, sets the root child to the program function, and prints the parse tree if the parse is successful.
node_t* parser(FILE* file){
	node_t *root;
	token_t token = scanner(file);
	root = program(&token, file);
	if(token.tokenID == EOF_tk){
		printf("Parse successful.\n");
		printParseTree(root, 0);
	}
	else{
		error(0, &token);
	}
	return root;
}

/*
	These are the nonterminal functions that follow the BNF. In each function, a new node is created using the getNode()
	function.  Based on the BNF, these functions get tokens from the scanner, set the node's children to other nonterminal
	functions, and store tokens in the node if needed.  If there is an error, the error function is called.  A node is returned
	(either NULL or the actual node) in each function.
*/
static node_t* program(token_t *token, FILE* file){
	node_t* node = getNode("program");	
	node->child1 = vars(token, file);
	node->child2 = block(token, file);
	return node;
}

static node_t* block(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "BEGIN") == 0){
		node_t *node = getNode("block");
		*token = scanner(file);
		node->child1 = vars(token, file);
		if(token->tokenID == COL_tk){
			*token = scanner(file);
			node->child2 = stats(token, file);
			if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "END") == 0){
				*token = scanner(file);
				return node;
			}
			else{
				error(2, token);
				return NULL;
			}
		}
		else{
			error(5, token);
			return NULL;
		}
	}
	else{
		error(1, token);
		return NULL;
	}
}

static node_t* vars(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "VOID") == 0){
		node_t *node = getNode("vars");
		*token = scanner(file);
		if(token->tokenID == ID_tk){
			node->token1 = getToken(token);
			*token = scanner(file);
			node->child1 = mvars(token, file);
			return node;
		}
		else{
			error(3, token);
			return NULL;
		}
	}
	else
		return NULL;

}

static node_t* mvars(token_t *token, FILE* file){
	if(token->tokenID == ID_tk){
		node_t *node = getNode("mvars");
		node->token1 = getToken(token);
		*token = scanner(file);	
		node->child1 = mvars(token, file);
		return node;
	}
	else
		return NULL;
}

static node_t* expr(token_t *token, FILE* file){
	node_t *node = getNode("expr");
	node->child1 = M(token, file);
	if(token->tokenID == PLUS_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		node->child2 = expr(token, file);
		return node;
	}
	else
		return node;
}

static node_t* M(token_t *token, FILE* file){
	node_t *node = getNode("M");
	node->child1 = T(token, file);
	if(token->tokenID == MIN_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		node->child2 = M(token, file);
		return node;
	}
	else
		return node;
}

static node_t* T(token_t *token, FILE* file){
	node_t *node = getNode("T");
	node->child1 = F(token, file);
	if(token->tokenID == MUL_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		node->child2 = T(token, file);
		return node;
	}
	else if(token->tokenID == DIV_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		node->child2 = T(token, file);
		return node;
	}
	else
		return node;	
}

static node_t* F(token_t *token, FILE* file){
	node_t *node = getNode("F");
	if(token->tokenID == MIN_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		node->child1 = F(token, file);
		return node;
	}
	else{
		node->child1 = R(token, file);
		return node;
	}
}

static node_t* R(token_t *token, FILE* file){
	if(token->tokenID == OPENPAR_tk){
		node_t *node = getNode("R");
		*token = scanner(file);
		node->child1 = expr(token, file);
		if(token->tokenID == CLOSEDPAR_tk){
			*token = scanner(file);
			return node;
		}
		else{
			error(4, token);
			return NULL;
		}
	}
	else if(token->tokenID == ID_tk){
		node_t *node = getNode("R");
		node->token1 = getToken(token);
		*token = scanner(file);
		return node;
	}
	else if(token->tokenID == DIGIT_tk){
		node_t *node = getNode("R");
		node->token1 = getToken(token);
		*token = scanner(file);
		return node;
	}
	else{
		error(8, token);
		return NULL;
	}
}

static node_t* stats(token_t *token, FILE* file){
	node_t *node = getNode("stats");
	node->child1 = stat(token, file);
	if(token->tokenID == COL_tk){
		*token = scanner(file);
		node->child2 = mStat(token, file);
		return node;
	}
	else{
		error(5, token);
		return NULL;
	}
}

static node_t* mStat(token_t *token, FILE* file){
	if((token->tokenID == KW_tk && strcmp(token->tokenInstance, "INPUT") == 0) || (token->tokenID == KW_tk && strcmp(token->tokenInstance, "OUTPUT") == 0) || (token->tokenID == KW_tk && strcmp(token->tokenInstance, "BEGIN") == 0) || (token->tokenID == KW_tk && strcmp(token->tokenInstance, "IF") == 0) || (token->tokenID == KW_tk && strcmp(token->tokenInstance, "FOR") == 0) || (token->tokenID == ID_tk)){
		node_t *node = getNode("mStat");
		node->child1 = stat(token, file);
		if(token->tokenID == COL_tk){
			*token = scanner(file);
			node->child2 = mStat(token, file);
			return node;
		}
		else{
			error(5, token);
			return NULL;
		}
	}
	else
		return NULL;
	
}

static node_t* stat(token_t *token, FILE* file){
	node_t *node = getNode("stat");
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "INPUT") == 0){
		node->child1 = in(token, file);
		return node;
	}
	else if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "OUTPUT") == 0){
		node->child1 = out(token, file);
		return node;
	}
	else if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "BEGIN") == 0){
		node->child1 = block(token, file);
		return node;
	}
	else if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "IF") == 0){
		node->child1 = iff(token, file);
		return node;
	}
	else if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "FOR") == 0){
		node->child1 = loop(token, file);
		return node;
	}
	else if(token->tokenID == ID_tk){
		node->child1 = assign(token, file);
		return node;
	}
	else{
		error(14, token);
		return NULL;
	}
}

static node_t* in(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "INPUT") == 0){
		node_t *node = getNode("in");
		*token = scanner(file);
		if(token->tokenID == ID_tk){
			node->token1 = getToken(token);
			*token = scanner(file);
			return node;
		}
		else{
			error(3, token);
			return NULL;
		}
	}
	else{
		error(9, token);
		return NULL;
	}
}

static node_t* out(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "OUTPUT") == 0){
		node_t *node = getNode("out");
		*token = scanner(file);
		node->child1 = expr(token, file);
		return node;
	}
	else{
		error(10, token);
		return NULL;
	}
}

static node_t* iff(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "IF") == 0){
		node_t *node = getNode("if");
		*token = scanner(file);
		if(token->tokenID == OPENPAR_tk){
			*token = scanner(file);
			node->child1 = expr(token, file);
			node->child2 = RO(token, file);
			node->child3 = expr(token, file);
			if(token->tokenID == CLOSEDPAR_tk){
				*token = scanner(file);
				node->child4 = block(token, file);
				return node;
			}
			else{
				error(4, token);
				return NULL;
			}
		}
		else{
			error(6, token);
			return NULL;
		}
	}
	else{
		error(11, token);
		return NULL;
	}
}

static node_t* loop(token_t *token, FILE* file){
	if(token->tokenID == KW_tk && strcmp(token->tokenInstance, "FOR") == 0){
		node_t *node = getNode("loop");
		*token = scanner(file);
		if(token->tokenID == OPENPAR_tk){
			*token = scanner(file);
			node->child1 = expr(token, file);
			node->child2 = RO(token, file);
			node->child3 = expr(token, file);
			if(token->tokenID == CLOSEDPAR_tk){
				*token = scanner(file);
				node->child4 = block(token, file);
				return node;
			}
			else{
				error(4, token);
				return NULL;
			}
		}
		else{
			error(6, token);
			return NULL;
		}
	}
	else{
		error(12, token);
		return NULL;
	}
}

static node_t* assign(token_t *token, FILE* file){
	if(token->tokenID == ID_tk){
		node_t *node = getNode("assign");
		node->token1 = getToken(token);
		*token = scanner(file);
		if(token->tokenID == EQ_tk){
			*token = scanner(file);
			node->child1 = expr(token, file);
			return node;
		}
		else{
			error(7, token);
			return NULL;
		}
	}
	else{
		error(3, token);
		return NULL;
	}
}

static node_t* RO(token_t *token, FILE* file){
	node_t *node = getNode("RO");
	if(token->tokenID == GREQGR_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		if(token->tokenID == EQ_tk){
			node->token2 = getToken(token);
			*token = scanner(file);
			return node;
		}
		else
			return node;
	}
	else if(token->tokenID == LSEQLS_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		if(token->tokenID == EQ_tk){
			node->token2 = getToken(token);
			*token = scanner(file);
			return node;
		}
		else
			return node;
	}
	else if(token->tokenID == NOT_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		return node;
	}
	else if(token->tokenID == EQ_tk){
		node->token1 = getToken(token);
		*token = scanner(file);
		if(token->tokenID == EQ_tk){
			node->token2 = getToken(token);
			*token = scanner(file);
			return node;
		}
		else{
			error(7, token);
			return NULL;
		}
	}
	else{
		error(13, token);
		return NULL;
	}
}

//Prints the errors based on the errorNumber that is passed to the function.
static void error(int errorNumber, token_t *token){
	switch (errorNumber){
		case 0:
			printf("%s\n", "ERRORS");
			exit(0);
		case 1:
			printf("%s%s%s%d\n", "Error: expected BEGIN, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 2:
			printf("%s%s%s%d\n", "Error: expected END, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 3:
			printf("%s%s%s%d\n", "Error: expected ID, received ", token->tokenInstance, " on line number: ", token->lineNumber);	
			exit(0);
		case 4:
			printf("%s%s%s%d\n", "Error: expected ), received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 5:
			printf("%s%s%s%d\n", "Error: expected :, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 6:
			printf("%s%s%s%d\n", "Error: expected (, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 7:
			printf("%s%s%s%d\n", "Error: expected =, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 8:
			printf("%s%s%s%d\n", "Error: expected ( or ID or number, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 9:
			printf("%s%s%s%d\n", "Error: expected INPUT, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 10:
			printf("%s%s%s%d\n", "Error: expected OUTPUT, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 11:
			printf("%s%s%s%d\n", "Error: expected IF, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 12:
			printf("%s%s%s%d\n", "Error: expected FOR, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 13:
			printf("%s%s%s%d\n", "Error: expected >=> or <=< or !! or =, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
		case 14:
			printf("%s%s%s%d\n", "Error: expected INPUT or OUTPUT or IF or FOR or Id, received ", token->tokenInstance, " on line number: ", token->lineNumber);
			exit(0);
	}
}

//Creates a new node. Uses the char* label that is passed as the node's label and then sets the tokens and children to NULL.
static node_t* getNode(char* label){

	node_t* node;
	node = (node_t*)malloc(sizeof(node_t));
	node->label = label;
	node->token1 = NULL;
	node->token2 = NULL;
	node->child1 = NULL;
	node->child2 = NULL;
	node->child3 = NULL;
	node->child4 = NULL;

	return node;
}

//Makes a copy of the token that is passed to it and returns this new copy.
static token_t* getToken(token_t *token){
	token_t* newToken;
	newToken = (token_t*)malloc(sizeof(token_t));
	newToken->tokenInstance = token->tokenInstance;
	newToken->tokenID = token->tokenID;
	newToken->lineNumber = token->lineNumber;
	return newToken;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

//----print parse tree function
/*
	The printParseTree function displays the parse tree in a preorder traversal.  It checks if the root is null and if it is
	it is there is an explicit return.  The tree is printed with indentation based on the level of the tree.
	Then the tokens are printed.  After that the parseTree is called with the root's children as parameters.
*/
void printParseTree(node_t *root, int level){
	if(root == NULL)
		return;
	printf("%*c%d:%-9s ", level*2, ' ', level, root->label);
	if(root->token1 != NULL)
		printf("TOKEN on line%d: %s", root->token1->lineNumber, root->token1->tokenInstance);
	if(root->token2 != NULL)
		printf(" %s", root->token2->tokenInstance);
	printf("\n");
	printParseTree(root->child1, level+1);
	printParseTree(root->child2, level+1);
	printParseTree(root->child3, level+1);
	printParseTree(root->child4, level+1);
}

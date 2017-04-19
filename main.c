#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "token.h"
#include "node.h"
#include "scanner.h"
#include "parser.h"
#include "sem.h"

/*
Main sets the file invocations.
The file can be open using the first argument after the executable name,
or it can be redirected.  The user can also input text directly instead.
Whichever of the three options the user selects, main passes a file pointer to parser().
*/
int main(int argc, char *argv[])
{
	char buff[1000];
	char *arg = (char *)malloc(100);
	node_t* root;

	//option 1 --- user specifies file without extension	
	if (argc == 2){
		arg = argv[1];
		strcat(arg, ".4280E02");

		FILE *file = fopen(arg, "r");
		if (file == NULL){
			printf("File cannot be opened, errno = %d\n", errno);
			return 1;
		}
		root = parser(file);
		sem(root);
		fclose(file);

	}

	//option 2 and 3 --- user redirects file or inputs text into stdin
	else if (argc == 1){
		int c;		
		FILE *file2;
		file2 = tmpfile();
	
		c = fgetc(stdin);	
		while(!feof(stdin)){
			ungetc(c, stdin);
			fgets(buff, 1000, stdin);
			fputs(buff, file2);
			c = fgetc(stdin);
		}

		rewind(file2);
		root = parser(file2);
		sem(root);
		fclose(file2);	
	}
	else{
		printf("%s\n", "Error: too many arguments.  Please enter 0 or 1 arguments.");
	}

	return 0;
}

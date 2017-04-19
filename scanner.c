#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include "token.h"

//----scanner file----

int lineno = 1;
static token_t FADriver(FILE*);
static void assignID(int, token_t*);
static char* printError(int);
static bool checkString(char*, token_t*);
static int getColumn(char, token_t*);


/*
----scanner function----
This function simply calls FADriver and returns the token received from that function.
*/
token_t scanner(FILE *fileName){
	token_t token;
	token = FADriver(fileName);
	return token;
}

/*
----FADriver function----
This function reads one character at a time and returns one token back to the scanner function if the characters that are read are match to a token.
If there is an error, the error is printed and the function exits.
*/
static token_t FADriver(FILE* file){

	state_t state=INITIAL;
	int nextState;
	char nextChar;
	int column;
	int table[29][23];
	token_t token;
	char c;
	int count;
	int position;

	token.tokenName[0] = "KeywordTk";
	token.tokenName[1] = "IDTk";
	token.tokenName[2] = "NumberTk";
	token.tokenName[3] = "EOFTk";
	token.tokenName[4] = "EqualTk";
	token.tokenName[5] = "GreaterThanEqualGreaterThanTk";
	token.tokenName[6] = "LessThanEqualLessThanTk";
	token.tokenName[7] = "NotTk";
	token.tokenName[8] = "ColonTk";
	token.tokenName[9] = "PlusTk";
	token.tokenName[10] = "MinusTk";
	token.tokenName[11] = "MultiplyTk";
	token.tokenName[12] = "DivideTk";
	token.tokenName[13] = "PeriodTk";
	token.tokenName[14] = "OpenParenthesisTk";
	token.tokenName[15] = "ClosedParenthesisTk";
	token.tokenName[16] = "CommaColon";
	token.tokenName[17] = "OpenCurlyBracketTk";
	token.tokenName[18] = "ClosedCurlyBracketTk";
	token.tokenName[19] = "SemicolonTk";
	token.tokenName[20] = "OpenSquareBracketTk";
	token.tokenName[21] = "ClosedSquareBracketTk";

	//the table is set with appropriate states
	table[0][0] = 13; 
	table[0][1] = 6;
	table[0][2] = 9;
	table[0][3] = 5;
	table[0][4] = 14;
	table[0][5] = 15;
	table[0][6] = 16;
	table[0][7] = 17;
	table[0][8] = 18;
	table[0][9] = 19;
	table[0][10] = 20;
	table[0][11] = 21;
	table[0][12] = 22;
	table[0][13] = 23;
	table[0][14] = 24;
	table[0][15] = 25;
	table[0][16] = 26;
	table[0][17] = 27;
	table[0][18] = 2;
	table[0][19] = 4;
	table[0][20] = 0;
	table[0][21] = 28;
	table[0][22] = -6;
	
	table[1][0] = 1;
	table[1][1] = 1;
	table[1][2] = 1;
	table[1][3] = 1;
	table[1][4] = 1;
	table[1][5] = 1;
	table[1][6] = 1;
	table[1][7] = 1;
	table[1][8] = 1;
	table[1][9] = 1;
	table[1][10] = 1;
	table[1][11] = 1;
	table[1][12] = 1;
	table[1][13] = 1;
	table[1][14] = 1;
	table[1][15] = 1;
	table[1][16] = 1;
	table[1][17] = 1;
	table[1][18] = 1;
	table[1][19] = 1;
	table[1][20] = 3;
	table[1][21] = 1;
	table[1][22] = 1;
	
	table[2][0] = 1018;
	table[2][1] = 1018;
	table[2][2] = 1018; 
	table[2][3] = 1018;
	table[2][4] = 1018;
	table[2][5] = 1018;
	table[2][6] = 1018;
	table[2][7] = 1018;
	table[2][8] = 1018;
	table[2][9] = 1018;
	table[2][10] = 1018;
	table[2][11] = 1018;
	table[2][12] = 1018;
	table[2][13] = 1018;
	table[2][14] = 1018;
	table[2][15] = 1018;
	table[2][16] = 1018;
	table[2][17] = 1018;
	table[2][18] = 2;
	table[2][19] = 2;
	table[2][20] = 1018;
	table[2][21] = 1018;
	table[2][22] = 1018;	

	table[3][0] = 1021;
	table[3][1] = 1021;
	table[3][2] = 1021;
	table[3][3] = 1021;
	table[3][4] = 1021;
	table[3][5] = 1021;
	table[3][6] = 1021;
	table[3][7] = 1021;
	table[3][8] = 1021;
	table[3][9] = 1021;
	table[3][10] = 1021;
	table[3][11] = 1021;
	table[3][12] = 1021;
	table[3][13] = 1021;
	table[3][14] = 1021;
	table[3][15] = 1021;
	table[3][16] = 1021;
	table[3][17] = 1021;
	table[3][18] = 1021;
	table[3][19] = 1021;
	table[3][20] = 1021;
	table[3][21] = 1021;
	table[3][22] = 1021;	

	table[4][0] = 1020;
	table[4][1] = 1020;
	table[4][2] = 1020;
	table[4][3] = 1020;
	table[4][4] = 1020;
	table[4][5] = 1020;
	table[4][6] = 1020;
	table[4][7] = 1020;
	table[4][8] = 1020;
	table[4][9] = 1020;
	table[4][10] = 1020;
	table[4][11] = 1020;
	table[4][12] = 1020;
	table[4][13] = 1020;
	table[4][14] = 1020;
	table[4][15] = 1020;
	table[4][16] = 1020;
	table[4][17] = 1020;
	table[4][18] = 1020;
	table[4][19] = 4;
	table[4][20] = 1020;
	table[4][21] = 1020;
	table[4][22] = 1020;	

	table[5][0] = -1;
        table[5][1] = -1;
        table[5][2] = -1;
        table[5][3] = 7;
        table[5][4] = -1;
        table[5][5] = -1;
        table[5][6] = -1;
        table[5][7] = -1;
        table[5][8] = -1;
        table[5][9] = -1;
        table[5][10] = -1;
        table[5][11] = -1;
        table[5][12] = -1;
        table[5][13] = -1;
        table[5][14] = -1;
        table[5][15] = -1;
        table[5][16] = -1;
        table[5][17] = -1;
        table[5][18] = -1;
        table[5][19] = -1;
        table[5][20] = -1;
        table[5][21] = -1;	
	table[5][22] = -1;

	table[6][0] = 8;
        table[6][1] = -2;
        table[6][2] = -2;
        table[6][3] = -2;
        table[6][4] = -2;
        table[6][5] = -2;
        table[6][6] = -2;
        table[6][7] = -2;
        table[6][8] = -2;
        table[6][9] = -2;
        table[6][10] = -2;
        table[6][11] = -2;
        table[6][12] = -2;
        table[6][13] = -2;
        table[6][14] = -2;
        table[6][15] = -2;
        table[6][16] = -2;
        table[6][17] = -2;
        table[6][18] = -2;
        table[6][19] = -2;
        table[6][20] = -2;
        table[6][21] = -2;
	table[6][22] = -2;

	table[7][0] = 1003;
        table[7][1] = 1003;
        table[7][2] = 1003;
        table[7][3] = 1003;
        table[7][4] = 1003;
        table[7][5] = 1003;
        table[7][6] = 1003;
        table[7][7] = 1003;
        table[7][8] = 1003;
        table[7][9] = 1003;
        table[7][10] = 1003;
        table[7][11] = 1003;
        table[7][12] = 1003;
        table[7][13] = 1003;
        table[7][14] = 1003;
        table[7][15] = 1003;
        table[7][16] = 1003;
        table[7][17] = 1003;
        table[7][18] = 1003;
        table[7][19] = 1003;
        table[7][20] = 1003;
        table[7][21] = 1003;	
	table[7][22] = 1003;

	table[8][0] = -3;
	table[8][1] = 10;
	table[8][2] = -3;
	table[8][3] = -3;
	table[8][4] = -3;
	table[8][5] = -3;
	table[8][6] = -3;
	table[8][7] = -3;
	table[8][8] = -3;
	table[8][9] = -3;
	table[8][10] = -3;
	table[8][11] = -3;
	table[8][12] = -3;
	table[8][13] = -3;
	table[8][14] = -3;
	table[8][15] = -3;
	table[8][16] = -3;
	table[8][17] = -3;
	table[8][18] = -3;
	table[8][19] = -3;
	table[8][20] = -3;
	table[8][21] = -3;
	table[8][22] = -3;

	table[9][0] = 11;
        table[9][1] = -4;
        table[9][2] = -4;
        table[9][3] = -4;
        table[9][4] = -4;
        table[9][5] = -4;
        table[9][6] = -4;
        table[9][7] = -4;
        table[9][8] = -4;
        table[9][9] = -4;
        table[9][10] = -4;
        table[9][11] = -4;
        table[9][12] = -4;
        table[9][13] = -4;
        table[9][14] = -4;
        table[9][15] = -4;
        table[9][16] = -4;
        table[9][17] = -4;
        table[9][18] = -4;
        table[9][19] = -4;
        table[9][20] = -4;
        table[9][21] = -4;
	table[9][22] = -4;

	table[10][0] = 1001;
        table[10][1] = 1001;
        table[10][2] = 1001;
        table[10][3] = 1001;
        table[10][4] = 1001;
        table[10][5] = 1001;
        table[10][6] = 1001;
        table[10][7] = 1001;
        table[10][8] = 1001;
        table[10][9] = 1001;
        table[10][10] = 1001;
        table[10][11] = 1001;
        table[10][12] = 1001;
        table[10][13] = 1001;
        table[10][14] = 1001;
        table[10][15] = 1001;
        table[10][16] = 1001;
        table[10][17] = 1001;
        table[10][18] = 1001;
        table[10][19] = 1001;
        table[10][20] = 1001;
        table[10][21] = 1001;
	table[10][22] = 1001;

	table[11][0] = -5;
	table[11][1] = -5;
	table[11][2] = 12;
	table[11][3] = -5;
	table[11][4] = -5;
	table[11][5] = -5;
	table[11][6] = -5;
	table[11][7] = -5;
	table[11][8] = -5;
	table[11][9] = -5;
	table[11][10] = -5;
	table[11][11] = -5;
	table[11][12] = -5;
	table[11][13] = -5;
	table[11][14] = -5;
	table[11][15] = -5;
	table[11][16] = -5;
	table[11][17] = -5;
	table[11][18] = -5;
	table[11][19] = -5;
	table[11][20] = -5;
	table[11][21] = -5;
	table[11][22] = -5;

	table[12][0] = 1002;
	table[12][1] = 1002;
	table[12][2] = 1002;
	table[12][3] = 1002;
	table[12][4] = 1002;
	table[12][5] = 1002;
	table[12][6] = 1002;
	table[12][7] = 1002;
	table[12][8] = 1002;
	table[12][9] = 1002;
	table[12][10] = 1002;
	table[12][11] = 1002;
	table[12][12] = 1002;
	table[12][13] = 1002;
	table[12][14] = 1002;
	table[12][15] = 1002;
	table[12][16] = 1002;
	table[12][17] = 1002;
	table[12][18] = 1002;
	table[12][19] = 1002;
	table[12][20] = 1002;
	table[12][21] = 1002;
	table[12][22] = 1002;

	//final =
	table[13][0] = 1000;
	table[13][1] = 1000;
	table[13][2] = 1000;
	table[13][3] = 1000;
	table[13][4] = 1000;
	table[13][5] = 1000;
	table[13][6] = 1000;
	table[13][7] = 1000;
	table[13][8] = 1000;
	table[13][9] = 1000;
	table[13][10] = 1000;
	table[13][11] = 1000;
	table[13][12] = 1000;
	table[13][13] = 1000;
	table[13][14] = 1000;
	table[13][15] = 1000;
	table[13][16] = 1000;
	table[13][17] = 1000;
	table[13][18] = 1000;
	table[13][19] = 1000;
	table[13][20] = 1000;
	table[13][21] = 1000;
	table[13][22] = 1000;

	//final :
	table[14][0] = 1004;
	table[14][1] = 1004;
	table[14][2] = 1004;
	table[14][3] = 1004;
	table[14][4] = 1004;
	table[14][5] = 1004;
	table[14][6] = 1004;
	table[14][7] = 1004;
	table[14][8] = 1004;
	table[14][9] = 1004;
	table[14][10] = 1004;
	table[14][11] = 1004;
	table[14][12] = 1004;
	table[14][13] = 1004;
	table[14][14] = 1004;
	table[14][15] = 1004;
	table[14][16] = 1004;
	table[14][17] = 1004;
	table[14][18] = 1004;
	table[14][19] = 1004;
	table[14][20] = 1004;
	table[14][21] = 1004;
	table[14][22] = 1004;

	//final +
	table[15][0] = 1005;
	table[15][1] = 1005;
	table[15][2] = 1005;
	table[15][3] = 1005;
	table[15][4] = 1005;
	table[15][5] = 1005;
	table[15][6] = 1005;
	table[15][7] = 1005;
	table[15][8] = 1005;
	table[15][9] = 1005;
	table[15][10] = 1005;
	table[15][11] = 1005;
	table[15][12] = 1005;
	table[15][13] = 1005;
	table[15][14] = 1005;
	table[15][15] = 1005;
	table[15][16] = 1005;
	table[15][17] = 1005;
	table[15][18] = 1005;
	table[15][19] = 1005;
	table[15][20] = 1005;
	table[15][21] = 1005;
	table[15][22] = 1005;

	//final -
	table[16][0] = 1006;
	table[16][1] = 1006;
	table[16][2] = 1006;
	table[16][3] = 1006;
	table[16][4] = 1006;
	table[16][5] = 1006;
	table[16][6] = 1006;
	table[16][7] = 1006;
	table[16][8] = 1006;
	table[16][9] = 1006;
	table[16][10] = 1006;
	table[16][11] = 1006;
	table[16][12] = 1006;
	table[16][13] = 1006;
	table[16][14] = 1006;
	table[16][15] = 1006;
	table[16][16] = 1006;
	table[16][17] = 1006;
	table[16][18] = 1006;
	table[16][19] = 1006;
	table[16][20] = 1006;
	table[16][21] = 1006;
	table[16][22] = 1006;

	//final *
	table[17][0] = 1007;
	table[17][1] = 1007;
	table[17][2] = 1007;
	table[17][3] = 1007;
	table[17][4] = 1007;
	table[17][5] = 1007;
	table[17][6] = 1007;
	table[17][7] = 1007;
	table[17][8] = 1007;
	table[17][9] = 1007;
	table[17][10] = 1007;
	table[17][11] = 1007;
	table[17][12] = 1007;
	table[17][13] = 1007;
	table[17][14] = 1007;
	table[17][15] = 1007;
	table[17][16] = 1007;
	table[17][17] = 1007;
	table[17][18] = 1007;
	table[17][19] = 1007;
	table[17][20] = 1007;
	table[17][21] = 1007;
	table[17][22] = 1007;
	
	//final /
	table[18][0] = 1008;
	table[18][1] = 1008;
	table[18][2] = 1008;
	table[18][3] = 1008;
	table[18][4] = 1008;
	table[18][5] = 1008;
	table[18][6] = 1008;
	table[18][7] = 1008;
	table[18][8] = 1008;
	table[18][9] = 1008;
	table[18][10] = 1008;
	table[18][11] = 1008;
	table[18][12] = 1008;
	table[18][13] = 1008;
	table[18][14] = 1008;
	table[18][15] = 1008;
	table[18][16] = 1008;
	table[18][17] = 1008;
	table[18][18] = 1008;
	table[18][19] = 1008;
	table[18][20] = 1008;
	table[18][21] = 1008;
	table[18][22] = 1008;

	//final .
	table[19][0] = 1009;
	table[19][1] = 1009;
	table[19][2] = 1009;
	table[19][3] = 1009;
	table[19][4] = 1009;
	table[19][5] = 1009;
	table[19][6] = 1009;
	table[19][7] = 1009;
	table[19][8] = 1009;
	table[19][9] = 1009;
	table[19][10] = 1009;
	table[19][11] = 1009;
	table[19][12] = 1009;
	table[19][13] = 1009;
	table[19][14] = 1009;
	table[19][15] = 1009;
	table[19][16] = 1009;
	table[19][17] = 1009;
	table[19][18] = 1009;
	table[19][19] = 1009;
	table[19][20] = 1009;
	table[19][21] = 1009;
	table[19][22] = 1009;

	//final (
	table[20][0] = 1010;
	table[20][1] = 1010;
	table[20][2] = 1010;
	table[20][3] = 1010;
	table[20][4] = 1010;
	table[20][5] = 1010;
	table[20][6] = 1010;
	table[20][7] = 1010;
	table[20][8] = 1010;
	table[20][9] = 1010;
	table[20][10] = 1010;
	table[20][11] = 1010;
	table[20][12] = 1010;
	table[20][13] = 1010;
	table[20][14] = 1010;
	table[20][15] = 1010;
	table[20][16] = 1010;
	table[20][17] = 1010;
	table[20][18] = 1010;
	table[20][19] = 1010;
	table[20][20] = 1010;
	table[20][21] = 1010;
	table[20][22] = 1010;
	
	//final )
	table[21][0] = 1011;
	table[21][1] = 1011;
	table[21][2] = 1011;
	table[21][3] = 1011;
	table[21][4] = 1011;
	table[21][5] = 1011;
	table[21][6] = 1011;
	table[21][7] = 1011;
	table[21][8] = 1011;
	table[21][9] = 1011;
	table[21][10] = 1011;
	table[21][11] = 1011;
	table[21][12] = 1011;
	table[21][13] = 1011;
	table[21][14] = 1011;
	table[21][15] = 1011;
	table[21][16] = 1011;
	table[21][17] = 1011;
	table[21][18] = 1011;
	table[21][19] = 1011;
	table[21][20] = 1011;
	table[21][21] = 1011;
	table[21][22] = 1011;

	//final ,
	table[22][0] = 1012;
	table[22][1] = 1012;
	table[22][2] = 1012;
	table[22][3] = 1012;
	table[22][4] = 1012;
	table[22][5] = 1012;
	table[22][6] = 1012;
	table[22][7] = 1012;
	table[22][8] = 1012;
	table[22][9] = 1012;
	table[22][10] = 1012;
	table[22][11] = 1012;
	table[22][12] = 1012;
	table[22][13] = 1012;
	table[22][14] = 1012;
	table[22][15] = 1012;
	table[22][16] = 1012;
	table[22][17] = 1012;
	table[22][18] = 1012;
	table[22][19] = 1012;
	table[22][20] = 1012;
	table[22][21] = 1012;
	table[22][22] = 1012;
	
	//final {
	table[23][0] = 1013;
	table[23][1] = 1013;
	table[23][2] = 1013;
	table[23][3] = 1013;
	table[23][4] = 1013;
	table[23][5] = 1013;
	table[23][6] = 1013;
	table[23][7] = 1013;
	table[23][8] = 1013;
	table[23][9] = 1013;
	table[23][10] = 1013;
	table[23][11] = 1013;
	table[23][12] = 1013;
	table[23][13] = 1013;
	table[23][14] = 1013;
	table[23][15] = 1013;
	table[23][16] = 1013;
	table[23][17] = 1013;
	table[23][18] = 1013;
	table[23][19] = 1013;
	table[23][20] = 1013;
	table[23][21] = 1013;
	table[23][22] = 1013;

	//final }
	table[24][0] = 1014;
	table[24][1] = 1014;
	table[24][2] = 1014;
	table[24][3] = 1014;
	table[24][4] = 1014;
	table[24][5] = 1014;
	table[24][6] = 1014;
	table[24][7] = 1014;
	table[24][8] = 1014;
	table[24][9] = 1014;
	table[24][10] = 1014;
	table[24][11] = 1014;
	table[24][12] = 1014;
	table[24][13] = 1014;
	table[24][14] = 1014;
	table[24][15] = 1014;
	table[24][16] = 1014;
	table[24][17] = 1014;
	table[24][18] = 1014;
	table[24][19] = 1014;
	table[24][20] = 1014;
	table[24][21] = 1014;
	table[24][22] = 1014;

	//final ;
	table[25][0] = 1015;
	table[25][1] = 1015;
	table[25][2] = 1015;
	table[25][3] = 1015;
	table[25][4] = 1015;
	table[25][5] = 1015;
	table[25][6] = 1015;
	table[25][7] = 1015;
	table[25][8] = 1015;
	table[25][9] = 1015;
	table[25][10] = 1015;
	table[25][11] = 1015;
	table[25][12] = 1015;
	table[25][13] = 1015;
	table[25][14] = 1015;
	table[25][15] = 1015;
	table[25][16] = 1015;
	table[25][17] = 1015;
	table[25][18] = 1015;
	table[25][19] = 1015;
	table[25][20] = 1015;
	table[25][21] = 1015;
	table[25][22] = 1015;

	//final [
	table[26][0] = 1016;
	table[26][1] = 1016;
	table[26][2] = 1016;
	table[26][3] = 1016;
	table[26][4] = 1016;
	table[26][5] = 1016;
	table[26][6] = 1016;
	table[26][7] = 1016;
	table[26][8] = 1016;
	table[26][9] = 1016;
	table[26][10] = 1016;
	table[26][11] = 1016;
	table[26][12] = 1016;
	table[26][13] = 1016;
	table[26][14] = 1016;
	table[26][15] = 1016;
	table[26][16] = 1016;
	table[26][17] = 1016;
	table[26][18] = 1016;
	table[26][19] = 1016;
	table[26][20] = 1016;
	table[26][21] = 1016;
	table[26][22] = 1016;

	//final ]
	table[27][0] = 1017;
	table[27][1] = 1017;
	table[27][2] = 1017;
	table[27][3] = 1017;
	table[27][4] = 1017;
	table[27][5] = 1017;
	table[27][6] = 1017;
	table[27][7] = 1017;
	table[27][8] = 1017;
	table[27][9] = 1017;
	table[27][10] = 1017;
	table[27][11] = 1017;
	table[27][12] = 1017;
	table[27][13] = 1017;
	table[27][14] = 1017;
	table[27][15] = 1017;
	table[27][16] = 1017;
	table[27][17] = 1017;
	table[27][18] = 1017;
	table[27][19] = 1017;
	table[27][20] = 1017;
	table[27][21] = 1017;
	table[27][22] = 1017;

	//final EOF
	table[28][0] = 1022;
	table[28][1] = 1022;
	table[28][2] = 1022;
	table[28][3] = 1022;
	table[28][4] = 1022;
	table[28][5] = 1022;
	table[28][6] = 1022;
	table[28][7] = 1022;
	table[28][8] = 1022;
	table[28][9] = 1022;
	table[28][10] = 1022;
	table[28][11] = 1022;
	table[28][12] = 1022;
	table[28][13] = 1022;
	table[28][14] = 1022;
	table[28][15] = 1022;
	table[28][16] = 1022;
	table[28][17] = 1022;
	table[28][18] = 1022;
	table[28][19] = 1022;
	table[28][20] = 1022;
	table[28][21] = 1022;
	table[28][22] = 1022;

	nextState = 0;
	
	char temp[2];
	temp[0] = c;
	temp[1] = '\0';

	char* string = (char *)malloc(100);
	strcpy(string, "");	

	while (state!=FINAL){
					
		nextChar = fgetc(file);
		if(nextChar == '$'){
			while(nextChar != ' ' && nextChar != '\n' && nextChar != '\t'){
				nextChar = fgetc(file);
			}
		}

		temp[0] = nextChar;
		column = getColumn(nextChar, &token);
		nextState=table[state][column];
		if (nextState < 0){
			printf("ERROR %s: %c on line number %d\n", printError(nextState), temp[0], lineno);
			exit(0);
		}
		else if (nextState >= 1000){
			if(nextChar == EOF){
				token.tokenInstance = "EOF";
			}
			else{
				token.tokenInstance = string;
			}
			token.lineNumber = lineno;
			assignID(nextState, &token);
			if (nextState == 1018){
				if (checkString(string, &token)){
					ungetc(nextChar, file);
					token.tokenID=KW_tk;
                        		return token;
				}
                 		else{				
  	                     		ungetc(nextChar, file);
					token.tokenID=ID_tk;
					return token;
				}
			}
			else{
				ungetc(nextChar, file);				
				return token;
			}
      		}
		else{
			if(column != 20){
				strcat(string, temp);
			}
			if(nextChar == '\n'){
				lineno++;
			}
			if(nextChar == EOF){
				lineno--;
			}
			state = nextState;
		}
	}
	if(token.tokenID == EOF_tk){
		fclose(file);
	}
	return token;

}

//prints errors based on error numbers
static char* printError(int errorNum){

	char* error;

	switch(errorNum){
		case -1:
			error = "no ! token";
			break;
		case -2:
			error = "no > token";
			break;
		case - 3:
			error = "no >= token";
			break;
		case -4:
			error = "no < token";
			break;
		case -5:
			error = "no <= token";
			break;
		case -6:
			error = "character not in alphabet";
			break;
	}
	return error;
}

//returns the column number for the current character
static int getColumn(char nextChar, token_t* token){

	int column = -1;

	if(isalpha(nextChar))
		nextChar = 'a';
	else if(isdigit(nextChar))
		nextChar = '1';
	else if(nextChar == EOF)
		nextChar = '2';
	
	switch (nextChar){
		case '=':
			column = 0;
			break;
		case '>':
			column = 1;
			break;
		case '<':
			column = 2;
			break;
		case '!':
			column = 3;
			break;
		case ':':
			column = 4;
			break;
		case '+':
			column = 5;
			break;
		case '-':
			column = 6;
			break;
		case '*':
			column = 7;
			break;
		case '/':
			column = 8;
			break;
		case '.':
			column = 9;
			break;
		case '(':
			column = 10;
			break;
		case ')':
			column = 11;
			break;
		case ',':
			column = 12;
			break;
		case '{':
			column = 13;
			break;
		case '}':
			column = 14;
			break;
		case ';':
			column = 15;
			break;
		case '[':
			column = 16;
			break;
		case ']':
			column = 17;
			break;
		case 'a':
			column = 18;
			break;
		case '1':
			column = 19;
			break;
		case '\n':
		case '\t':
		case ' ':
			column = 20;
			break;
		case '2':
			column = 21;
			break;
		default:
			column = 22;
	}

	return column;
}

//assigns tokenids based on the state
static void assignID(int nextState, token_t *token){

	switch (nextState){
		case 1000:
			token->tokenID = EQ_tk;
			break;
		case 1001:
			token->tokenID = GREQGR_tk;
			break;
		case 1002:
			token->tokenID = LSEQLS_tk;
			break;
		case 1003:
			token->tokenID = NOT_tk;
			break;
		case 1004:
			token->tokenID = COL_tk;
			break;
		case 1005:
			token->tokenID = PLUS_tk;
			break;
		case 1006:
			token->tokenID = MIN_tk;
			break;
		case 1007:
			token->tokenID = MUL_tk;
			break;
		case 1008:
			token->tokenID = DIV_tk;
			break;
		case 1009:
			token->tokenID = PERIOD_tk;
			break;
		case 1010:
			token->tokenID = OPENPAR_tk;
			break;
		case 1011:
			token->tokenID = CLOSEDPAR_tk;
			break;
		case 1012:
			token->tokenID = COMMA_tk;
			break;
		case 1013:
			token->tokenID = OPENCURBRACK_tk;
			break;
		case 1014:
			token->tokenID = CLOSEDCURBRACK_tk;
			break;
		case 1015:
			token->tokenID = SEMIC_tk;
			break;
		case 1016:
			token->tokenID = OPENSQBRACK_tk;
			break;
		case 1017:
			token->tokenID = CLOSEDSQBRACK_tk;
			break;
		case 1020:
			token->tokenID = DIGIT_tk;
			break;
		case 1022:
			token->tokenID = EOF_tk;
			break;
	}

}

//checks the string to see if the token is a keyword
static bool checkString(char* string, token_t *token){
	if(string[0] == 'B' && string[1] == 'E' && string[2] == 'G' && string[3] == 'I' && string[4] == 'N' && string[5] == '\0'){
		return true;
	}
	else if(string[0] == 'E' && string[1] == 'N' && string[2] == 'D' && string[3] == '\0'){
		return true;
	}
	else if(string[0] == 'I' && string[1] == 'F' && string[2] == '\0'){
		return true;
	}
	else if(string[0] == 'F' && string[1] == 'O' && string[2] == 'R' && string[3] == '\0'){
		return true;
	}
	else if(string[0] == 'V' && string[1] == 'O' && string[2] == 'I' && string[3] == 'D' && string[4] == '\0'){
		return true;
	}
	else if(string[0] == 'V' && string[1] == 'A' && string[2] == 'R' && string[3] == '\0'){
		return true;
	}
	else if(string[0] == 'R' && string[1] == 'E' && string[2] == 'T' && string[3] == 'U' && string[4] == 'R' && string[5] == 'N' && string[6] == '\0'){
		return true;
	}
	else if(string[0] == 'I' && string[1] == 'N' && string[2] == 'P' && string[3] == 'U' && string[4] == 'T' && string[5] == '\0'){
		return true;
	}
	else if(string[0] == 'O' && string[1] == 'U' && string[2] == 'T' && string[3] == 'P' && string[4] == 'U' && string[5] == 'T' && string[6] == '\0'){
		return true;
	}
	return false;
}

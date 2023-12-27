/*
 * Parser.c
 *
 * the purpose of the code in this file is to parse input from the user,
 * pass the commands and arguments to the functions that handle them (if they are valid),
 * or deal with invalid or misformatted commands.
 *
 * this file contains:
 * 1. auxiliary functions that support the function parsing functions. (private)
 * 2. the parsing function, parseGameLoop. (public)
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AuxMethods.h"
#include "DataStructures.h"
#include "Game.h"
#include "Parser.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    1. AUXILIARY FUNCTIONS                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* caseSolve:
 * ----------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'solve'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseSolve(char * token){

	int i;
	char * n = 0;
	token = strtok(NULL, " \r\t\n");
	i=0;
	while( token != NULL ) {
		i++;
		if (i == 1) {
			n = token;
			break;}
	}
	if (i<1) {
		printf("ERROR: invalid command\n");
		return;	}
	solve(n);
	return;
}

/* caseEdit:
 * ---------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'edit'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseEdit(char * token){
	token = strtok(NULL, " \r\t\n");
	if (token == NULL)
		edit();
	else
		edit1(token);
	return;}

/* caseMarkErrors:
 * ---------------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'mark_errors'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseMarkErrors(char * token){
	int mark;
	if (myGame.mode != Solve) {
		printf("ERROR: invalid command\n");
		return;}
	token = strtok(NULL, " \r\t\n");
	if (token == NULL) {
		printf("ERROR: invalid command\n");
		return;}
	mark = parseMarkErrorInput(token);
	if (mark == -1) {
		printf("Error: the value should be 0 or 1\n");
		return;
	}
	myGame.markErrors = mark;
	return;
}

/* casePrintBoard:
 * ---------------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'print_board'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void casePrintBoard(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;}
	else {
		printgb();
		return;	}
}

/* caseSet:
 * --------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'set'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseSet(char * token){
	int i, f1, f2, f3, n1, n2, n3;
	if (myGame.mode == Init)
		printf("ERROR: invalid command\n");
	else{
		/*find out whether there are 3 valid int arguments*/
		i=0;
		while( token != NULL ) {
			token = strtok(NULL, " \r\t\n");
			if (token != NULL){
				i++;
				if (i==1){
					f1 = isNum(token);
					if (f1){
						n1 = atoi(token);
						f1 = ((n1>0) && (n1<=myGame.N));
					}
				}
				if (i==2){
					f2 = isNum(token);
					if (f2){
						n2 = atoi(token);
						f2 = ((n2>0) && (n2<=myGame.N));
					}
				}
				if (i==3){
					f3 = isNum(token);
					if (f3){
						n3 = atoi(token);
						f3 = ((n3>=0) && (n3<=myGame.N));
					}
					break;
				}
			}
		}
		if ( i == 3 && (!f1 || !f2 || !f3)) { /*i indicates number of arguments, and f1-3 tell whether they are ints*/
			printf("Error: value not in range 0-%d\n",myGame.N);
			return;	}
		if ((i<3)) {
			printf("Error: invalid command\n");
			return;	}
		else {
			set(n1,n2,n3);
			return;	}
	}
	return;
}

/* caseValidate:
 * -------------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'validate'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseValidate(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;}
	else {
		validate();
		return;}
}

/* caseGenerate:
 * -------------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'generate'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseGenerate(char * token){
	int i, f1, f2, n1, n2;
	if (myGame.mode != Edit) {
		printf("ERROR: invalid command\n");
		return;}
	else{
		i=0;
		/*find out whether there are 2 int argumetns*/
		while( token != NULL ) {
			token = strtok(NULL, " \r\t\n");
			if (token != NULL){
				i++;
				if (i==1) {
					f1 = isNum(token);
					if (f1) {
						n1 = atoi(token);
						f1 = ((n1>=0) && (n1<=myGame.N*myGame.N - myGame.numFilled));}
				}

				if (i==2){
					f2 = isNum(token);
					if (f2) {
						n2 = atoi(token);
						f2 = ((n2>=0) && (n2<=myGame.N*myGame.N - myGame.numFilled));}
					break;
				}
			}
		}
		if (i == 2 && (!f1 || !f2)) { /*i indicates number of arguments, and f1-2 tell whether they are ints*/
			printf("Error: value not in range 0-%d\n", myGame.N*myGame.N - myGame.numFilled);
			return;}
		if (i<2) {
			printf("Error: invalid command\n");
			return;}
		else
			generate(n1,n2);
	}
	return;
}

/* caseUndo:
 * ---------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'undo'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseUndo(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;
	}
	else{
		undo();
		return;}
}

/* caseRedo:
 * ---------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'redo'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseRedo(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;}
	else{
		redo();
		return;}
}

/* caseSave:
 * ---------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'save'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseSave(char * token){
	int i;
	char * n;
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;
	}
	else{
		i=0;
		while( token != NULL ) {
			token = strtok(NULL, " \r\t\n");
			if (token != NULL){
				i++;
				if (i==1){
					n = token;
					break;}
			}
		}
		if (i<1) {
			printf("ERROR: invalid command\n");
			return;}
		save(n);
		return;
	}
}

/* caseHint:
 * ---------
 * arguments:
 * token - a string to be parsed.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'hint'.
 * if input is misformatted, or if the arguments are not valid,
 * prints an error message.
 * if the input specifies a valid command, calls the function handling the command,
 * and passing the parsed arguments.
 */
void caseHint(char * token){
	int i, n1, n2, f1, f2;
	if (myGame.mode != Solve){
		printf("ERROR: invalid command\n");
		return;}

	/*find out whether there are 2 int arguments*/
	i=0;
	while( token != NULL ) {
		token = strtok(NULL, " \r\t\n");
		if (token != NULL){
			i++;
			if (i==1) {
				f1 = isNum(token);
				if (f1) {
					n1 = atoi(token);
					f1 = ((n1>0) && (n1<=myGame.N));
				}
			}
			if (i==2){
				f2 = isNum(token);
				if (f2) {
					n2 = atoi(token);
					f2 = ((n2>0) && (n2<=myGame.N));
				}
				break;
			}
		}
	}
	if (i == 2 && (!f1 || !f2)) { /*i indicates number of arguments, and f1-2 tell whether they are ints*/
		printf("Error: value not in range 1-%d\n", myGame.N);
		return;}
	if (i<2) {
		printf("Error: invalid command\n");
		return;}
	else {
		hint(--n1,--n2);
		return;
	}
}

/* caseNumSolutions:
 * -----------------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'num_solutions'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseNumSolutions(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;
	}
	else{
		numSolutions();
		return;}
}

/* caseAutofill:
 * -------------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'autofill'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseAutofill(){
	if (myGame.mode != Solve) {
		printf("ERROR: invalid command\n");
		return;
	}
	else{
		autofill();
		return;}
}

/* caseReset:
 * ----------
 * arguments:
 * none.
 * description:
 * parses input farther, if the first few characters in the input,
 * specify the case 'reset'.
 * if input is misformatted prints an error message.
 * if the input specifies a valid command, calls the function handling the command.
 */
void caseReset(){
	if (myGame.mode == Init) {
		printf("ERROR: invalid command\n");
		return;
	}
	else{
		reset();
		return;}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    2. THE PARSING FUNCTION                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* parseGameLoop:
 * --------------
 * arguments:
 * token - a string to be parsed.
 * description:
 * finds out whether input contains a valid command (and valid arguments).
 * if no valid command (and arguments) was found:
 *     prints an error message and returns.
 * else:
 *     calls the method that handles the command that was inputted, along
 *     with the arguments, if there are any.
 */
int parseGameLoop(char input[]){
	int inputHasNewLine = 0;
	inputHasNewLine = hasNewLine(input);
	if (!inputHasNewLine) {
		printf("ERROR: invalid command\n");
		goto END;}
	if (input[0] != '\0' && inputHasNewLine){
		char * token0;
		token0 = strtok(input, " \r\t\n");
		if (token0==NULL) {
			goto END;	}
		/*CASE 1: SOLVE*/
		if (token0[0] == 's' && token0[1] == 'o' && token0[2] == 'l' && token0[3] == 'v' && token0[4] == 'e' && (int)token0[5] == 0){
			caseSolve(token0);
			goto END;}
		/*CASE 2: EDIT*/
		if (token0[0] == 'e' && token0[1] == 'd' && token0[2] == 'i' && token0[3] == 't' && (int)token0[4] == 0){
			caseEdit(token0);
			goto END;}
		/*CASE 3: MARK_ERRORS*/
		if (token0[0] == 'm' && token0[1] == 'a' && token0[2] == 'r' && token0[3] == 'k' && token0[4] == '_' && token0[5] == 'e' && token0[6] == 'r' && token0[7] == 'r' && token0[8] == 'o' && token0[9] == 'r' && token0[10] == 's' && (int)token0[11] == 0){
			caseMarkErrors(token0);
			goto END;}
		/*CASE 4: PRINT_BOARD*/
		if (token0[0] == 'p' && token0[1] == 'r' && token0[2] == 'i' && token0[3] == 'n'&& token0[4] == 't' && token0[5] == '_' && token0[6] == 'b' &&  token0[7] == 'o' && token0[8] == 'a' && token0[9] == 'r' && token0[10] == 'd' && (int)token0[11] == 0){
			casePrintBoard();
			goto END;}
		/*CASE 5: SET*/
		if (token0[0] == 's' && token0[1] == 'e' && token0[2] == 't' && (int)token0[3] == 0){
			caseSet(token0);
			goto END;}
		/*CASE 6: VALIDATE*/
		if (token0[0] == 'v' && token0[1] == 'a' && token0[2] == 'l' && token0[3] == 'i'&& token0[4] == 'd' && token0[5] == 'a' && token0[6] == 't' &&  token0[7] == 'e' && (int)token0[8] == 0){
			validate();
			goto END;}
		/*CASE 7: GENERATE*/
		if (token0[0] == 'g' && token0[1] == 'e' && token0[2] == 'n' && token0[3] == 'e'&& token0[4] == 'r' && token0[5] == 'a' && token0[6] == 't' &&  token0[7] == 'e' && (int)token0[8] == 0){
			caseGenerate(token0);
			goto END;}
		/*CASE 8: UNDO*/
		if (token0[0] == 'u' && token0[1] == 'n' && token0[2] == 'd' && token0[3] == 'o' && (int)token0[4] == 0){
			caseUndo();
			goto END;}
		/*CASE 9: REDO*/
		if (token0[0] == 'r' && token0[1] == 'e' && token0[2] == 'd' && token0[3] == 'o' && (int)token0[4] == 0){
			caseRedo();
			goto END;}
		/*CASE 10: SAVE*/
		if (token0[0] == 's' && token0[1] == 'a' && token0[2] == 'v' && token0[3] == 'e' && (int)token0[4] == 0){
			caseSave(token0);
			goto END;}
		/*CASE 11: HINT*/
		if (token0[0] == 'h' && token0[1] == 'i' && token0[2] == 'n' && token0[3] == 't' && (int)token0[4] == 0){
			caseHint(token0);
			goto END;}
		/*CASE 12: NUM_SOLUTIONS*/
		if (token0[0] == 'n' && token0[1] == 'u' && token0[2] == 'm' && token0[3] == '_'&& token0[4] == 's' && token0[5] == 'o' && token0[6] == 'l' &&  token0[7] == 'u' && token0[8] == 't' && token0[9] == 'i' && token0[10] == 'o' && token0[11] == 'n' && token0[12] == 's' && (int)token0[13] == 0){
			caseNumSolutions();
			goto END;}
		/*CASE 13: AUTOFILL*/
		if (token0[0] == 'a' && token0[1] == 'u' && token0[2] == 't' && token0[3] == 'o'&& token0[4] == 'f' && token0[5] == 'i' && token0[6] == 'l' &&  token0[7] == 'l' && (int)token0[8] == 0){
			caseAutofill();
			goto END;}
		/*CASE 14: RESET*/
		if (token0[0] == 'r' && token0[1] == 'e' && token0[2] == 's' && token0[3] == 'e'&& token0[4] == 't' && (int)token0[5] == 0){
			caseReset();
			goto END;}
		/*CASE 15: EXIT*/
		if (token0[0] == 'e' && token0[1] == 'x' && token0[2] == 'i' && token0[3] == 't'&& (int)token0[4] == 0){
			exitGame();}
		printf("ERROR: invalid command\n");}
	END:
	if (feof(stdin)) {
		exitGame();}
	if (inputHasNewLine) {
		return 1;}
	do {
		fgets(input, 256, stdin);
		inputHasNewLine = hasNewLine(input);
	} while (!inputHasNewLine);
	if (feof(stdin)) {
		exitGame();}
	return 1;
}


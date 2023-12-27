/*
 * Game.c
 *
 * the functions in this file deal with the user-inputted command,
 * once the commands and their arguments have been parsed and
 * recognized as such. each function has a name corresponding to
 * the command it is dealing with.
 * additionaly, this file contains a function (gameLoop) that
 * manages the game flow - recieveing input from the user and
 * passing it to the parser.
 *
 * contents:
 * 1. command-handling functions. (public)
 * 2. game-flow function - gameLoop. (public)
 *
 */

#include "DataStructures.h"
#include "AuxMethods.h"
#include "Validate.h"
#include "Parser.h"
#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                 1. COMMAND HANDLING FUNCTIONS                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * solve:
 * ------
 * arguments:
 * file_name - a string containing the address of the file to load from.
 * description:
 * a method handling the user-command 'solve'.
 * loads a board in solve mode from the file "file_name".
 * reports error to the user if the loading process fails.
 */
void solve(char *file_name){
	FILE *ifp = NULL;
	int m,n,i,j;
	Cell *cell;
	errno = 0;
	ifp = fopen(file_name, "r");
	if (errno) {
		printf("Error: File doesn't exist or cannot be opened\n");
		return;
	}
	if (ifp == NULL) {
		printf("Error: File doesn't exist or cannot be opened\n");
		fclose(ifp);
		return;
	} else {
		/*get board dimensions and create board*/
		m = readSize(ifp);
		n = readSize(ifp);
		createGame(n, m, Solve);
		/*read and fill cells*/
		for (i=0; i<myGame.N; i++){ /* row i*/
			for (j=0; j<myGame.N; j++){ /*column j*/
				cell = getCell(j,i);
				readNextCell(ifp,cell);
				checkIfError(j, i, cell->val, 0);
			}
		}
	}
	fclose(ifp);
	printgb();
}

/*
 * edit:
 * -----
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'edit', when it doesnt
 * have an additional file address argument.
 * creates a 3x3 board in edit mode and prints the board.
 */
void edit(){
	createGame(3,3,Edit);
	printgb();
}

/*
 * edit1:
 * ------
 * arguments:
 * file_name - a string containing the address of the file to load from.
 * description:
 * a method handling the user-command 'edit', when a file address
 * is specified.
 * loads a board in edit mode from the file "file_name".
 * reports error to the user if the loading process fails.
 */
void edit1(char *file_name){
	FILE *ifp = NULL;
	int m,n,i,j;
	Cell *cell;

	errno = 0;
	ifp = fopen(file_name, "r");
	if (errno) {
		printf("Error: File cannot be opened\n");
		return;
	}
	if (ifp == NULL) {
		printf("Error: File cannot be opened\n");
		fclose(ifp);
		return;
	}

	else{
		/*get board dimensions and create board*/
		m = readSize(ifp);
		n = readSize(ifp);
		createGame(n, m, Edit);
		/*read and fill cells*/
		for (i=0; i<myGame.N; i++){
			for (j=0; j<myGame.N;j++){
				cell = getCell(j,i);
				readNextCell(ifp,cell);
				cell->isFixed = 0;
				checkIfError(j, i, cell->val, 0);
			}
		}
	}
	fclose(ifp);
	printgb();
}

/*
 * set:
 * ----
 * arguments:
 * n1 - cell's column.
 * n2 - cell's row.
 * n3 - cell's new value.
 * description:
 * a method handling the user-command 'set'.
 * sets cell <n1,n2> to value n3.
 * prints the relevant error messages if necessary.
 * updates the relevant data structures.
 */
void set(int n1,int n2,int n3){
	if (((n1<1) || (n1>myGame.N)) || ((n2<1) || (n2>myGame.N)) || ((n3<0) || (n3>myGame.N)))
		printf("Error: value not in range 0-%d\n",myGame.N);
	else{
		--n2;
		--n1;
		if ((getCell(n1,n2)->isFixed))
			printf("Error: cell is fixed\n");
		else{
			setCell(n1,n2,n3,1);
			if (myGame.mode == Init || myGame.numFilled == myGame.N*myGame.N) {
				return;
			}
			printgb();
		}
	}
}

/*
 * validate:
 * ---------
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'validate'.
 * if board is erroneus, prints error message and returns.
 * solves the board using ILP (by calling ILPvalidate).
 * reports result to the user.
 */
void validate() {

	/*check whether board is erroneous*/
	if (isErroneous()) {
		printf("Error: board contains erroneous values\n");
		return;
	}

	/*use ILP to solve the board, and report result to the user*/
	clearDemiVals(0);
	if (ILPvalidate()) {
		printf("Validation passed: board is solvable\n");
	} else {
		printf("Validation failed: board is unsolvable\n");
	}

}

/*
 * generate:
 * ---------
 * arguments:
 * x - number of cells to be randomly filled.
 * y - number of cells not to erase.
 * description:
 * a method handling the user-command 'generate'.
 * if arguments are not valid, or board is not empty, prints error message and returns.
 * tries to fill the board by calling tryGenerate(x,y). if generation fails,
 * it tries again. if 1000 attempts fail, prints a failure message and returns.
 */
void generate(int x,int y) {
	int counter = 0, success = 0, numEmptyCells;

	/*check the prerequisites*/
	numEmptyCells = myGame.N*myGame.N - myGame.numFilled;
	if (x > numEmptyCells || x < 0 || y < 0 || y > numEmptyCells) {
		printf("Error: value not in range 0-%d\n", numEmptyCells);
		return;
	}
	if (myGame.numFilled != 0) {
		printf("Error: board is not empty\n");
		return;
	}
	/*try to generate:*/
	while (counter < 1000) {
		success = tryGenerate(x,y);
		if (success) {
			printgb();
			return;
		}
		counter++;
	}
	printf("Error: puzzle generator failed\n");
}

/*
 * undo:
 * -----
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'undo'.
 * if there are no moves to undo. prints an error message and returns.
 * undoes the last move, and reports the changes to the user.
 */
void undo(void) {

	Stack *stack = safealloc(sizeof(Stack));
	Elem *e = NULL;
	stack->numOfElements = 0;

	/*check preconditions*/
	if (myGame.currMove == myGame.sentinel) {
		printf("Error: no moves to undo\n");
		return;
	}

	/*undo moves (and record the undone moves in stack)*/
	do {
		push(stack, 0);
		e = stack->top;
		e->move = myGame.currMove;
		undoAny();
	} while (myGame.currMove->user == 0 && myGame.currMove != myGame.sentinel);

	/*print the undone moves*/
	printgb();
	do {
		e = stack->top;
		printUndo(e->move);
		e = pop(stack);
		free(e);
	} while (stack->numOfElements > 0);

	free(stack);

	/*if this is the last cell to be filled*/
	if ((myGame.mode == Solve) && (myGame.numFilled == myGame.N*myGame.N)) {
		if (!ILPvalidate()) /*validation failed*/
			printf("Puzzle solution erroneous\n");
		else{
			printf("Puzzle solved successfully\n");
			myGame.mode = Init;
		}
	}
}

/*
 * redo:
 * -----
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'redo'.
 * if there are no moves to redo. prints an error message and returns.
 * redoes the last move, and reports the changes to the user.
 */
void redo(void) {
	Stack *stack1 = malloc(sizeof(Stack));
	Stack *stack2 = malloc(sizeof(Stack));
	Elem *e = NULL;
	stack1->numOfElements = 0;
	stack2->numOfElements = 0;

	/*check preconditions*/
	if (myGame.currMove->next == NULL) {
		printf("Error: no moves to redo\n");
		goto END;
	}

	/*redo a move*/
	do {
		redoAny();
		push(stack1, 0);
		stack1->top->move = myGame.currMove;
	} while (myGame.currMove->user == 0);

	/*create a second stack, which is the reverse of the first*/
	do {
		push(stack2, 0);
		stack2->top->move = stack1->top->move;
		e = pop(stack1);
		free(e);
	} while (stack1->numOfElements > 0);

	/*print to the user*/
	printgb();
	do {
		printRedo(stack2->top->move);
		e = pop(stack2);
		free(e);
	} while (stack2->numOfElements > 0);

	END:

	free(stack1);
	free(stack2);

	/*if this is the last cell to be filled*/
	if ((myGame.mode == Solve) && (myGame.numFilled == myGame.N*myGame.N)) {
		if (!ILPvalidate()) /*validation failed*/
			printf("Puzzle solution erroneous\n");
		else{
			printf("Puzzle solved successfully\n");
			myGame.mode = Init;
		}
	}
}

/*
 * save:
 * -----
 * address - a string containing file-address to save into.
 * a method handling the user-command 'save'.
 * saves the board into the file specified by 'address'.
 * if an error occurs, reports it the user and returns.
 */
void save(char* address) {

	FILE *ifp = NULL;
	Cell *cell;
	int f,i,j,val;

	/*check preconditions for EDIT mode*/
	if (myGame.mode == Edit){
		if (isErroneous()){
			printf("Error: board contains erroneous values\n");
			return;}
		if (!(ILPvalidate())){
			printf("Error: board validation failed\n");
			return;}
	}

	/*attempt to open file*/
	ifp = fopen(address, "w");
	if (!ifp){
		printf("Error: File cannot be created or modified\n");
		return;}
	if (!(writeSize(ifp, myGame.blockH, myGame.blockW)))
		return;

	/*if in EDIT mode, all filled cells should be saved as fixed*/
	if (myGame.mode == Edit) {
		for (i = 0; i < myGame.N*myGame.N; i++) {
			cell = getCellN(i);
			if (cell->val) {
				cell->isFixed = 1;
			} else {
				cell->isFixed = 0;
			}
		}
	}

	/*save game*/
	for (i=0; i<myGame.N;i++){ /*rows*/
		for (j=0; j<myGame.N - 1;j++) { /*columns*/
			cell = getCell(j,i);
			val = cell->val;
			if (!(writeCell(ifp,cell,val))) {
				return;
			}
		}
		/*save last cell in a line*/
		cell = getCell(j,i);
		val = cell->val;
		if (!(writeCellEndOfLine(ifp,cell,val)))
			return;
		f = fputc('\n', ifp);
		if (!f) {
			return;
		}
	}

	/*if in edit mode, revert all cells to being unfixed*/
	if (myGame.mode == Edit) {
		for (i = 0; i < myGame.N*myGame.N; i++) {
			getCellN(i)->isFixed = 0;	}
	}
	fclose(ifp);
	printf("Saved to: %s\n",address);
}

/*
 * hint:
 * -----
 * arguments:
 * col - hinted cell's column.
 * row - hinted cell's row.
 * description:
 * a method handling the user-command 'hint'.
 * if the arguments are not valid, board is erroneus, cell is fixed
 * or already contains a value, prints error message and returns.
 * solves the board using ILP.
 * if board is solvable:
 *     hints the user for the value of cell <col,row>/
 * else:
 *     prints an error message.
 */
void hint(int col, int row) {
	int N = myGame.N;
	int solvable;

	/*check preconditions*/

	if (col < 0 || col >= N || row < 0 || row >= N) {
		printf("Error: value not in range 1-%d\n", N);
		return;
	}

	if (isErroneous()) {
		printf("Error: board contains erroneous values\n");
		return;
	}

	if (getCell(col,row)->isFixed) {
		printf("Error: cell is fixed\n");
		return;
	}

	if (getCell(col,row)->val) {
		printf("Error: cell already contains a value\n");
		return;
	}

	/*do ILP solve*/

	clearDemiVals(0);
	solvable = ILPvalidate();

	if (!solvable) {
		printf("Error: board is unsolvable\n");
		return;
	}

	/*print result*/

	printf("Hint: set cell to %d\n", getCell(col, row)->demiVal);

}

/*
 * numSolutions:
 * -------------
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'num_solutions'.
 * if the board is erroneous, prints an error message and returns.
 * finds the number of solutions for the board, using a backtracking
 * algorithm with a stack.
 * reports the result the user.
 */
void numSolutions(void) {

	int counter = 0, index = nextEmptyCell(0), N = myGame.N;
	Stack *st = malloc(sizeof(Stack));
	Elem *e = NULL;

	/*check preconditions*/
	if (isErroneous()) {
		printf("Error: board contains erroneous values\n");
		return;
	}

	/*find no. of solutions*/
	st->numOfElements = 0;
	clearDemiVals(0);
	push(st, index);
	while (st->numOfElements > 0) {
		e = st->top;
		index = e->cellIndex;
		if (index >= N*N) { /*board is filled with a valid solution*/
			counter++;
			e = pop(st);
			free(e);
			continue;
		}
		if (getCellN(index)->val) { /*if cell already contains a value, find the next empty cell*/
			index = nextEmptyCell(index+1);
			push(st, index);
			continue;}
		while (1) {
			e = st->top;
			index = e->cellIndex;
			clearDemiVals(index + 1);
			copyValsToDemivals();
			getCellN(index)->demiVal++;
			if (getCellN(index)->demiVal > N) { /*it means we have exhausted the options for this cell, and should backtrack*/
				e = pop(st);
				free(e);
				break;}
			if (isDemiValid(index)) { /*if demiVal is valid, we move to the next empty cell*/
				index = nextEmptyCell(index+1);
				push(st, index);
				break;}
		}
	}

	/*report result to the user*/
	printf("Number of solutions: %d\n", counter);
	if (counter == 1) {
		printf("This is a good board!\n");}
	if (counter > 1) {
		printf("The puzzle has more than 1 solution, try to edit it further\n");}
	free(st);
}

/*
 * autofill:
 * ---------
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'autofill'.
 * if the board is erroneous, prints an error message and returns.
 * fills all cells in the board that have only 1 valid option.
 */
void autofill(void) {

	int last = -1;
	int index = 0;
	int N = myGame.N;
	int v = 0;
	int val = 0;
	Cell *cell;
	/*check preconditions*/
	if (isErroneous()) {
		printf("Error: board contains erroneous values\n");
		return;
	}
	/*if there is a single value for a cell, save it in cell->demiVal*/
	clearDemiVals(0);
	while (1) {
		if (index >= N*N) {
			break;}
		cell = getCellN(index);
		if (cell->val) {
			index++;
			continue;}
		val = getCellSingleValue(index);
		if (val == 0) {
			index++;
			continue;	}
		cell->demiVal = val;
		last = index;
		index++;}
	/*for every cell with a saved demiVals, set the cell to this value*/
	if (last < 0) {
		printgb();
		return;
	}
	for (index = 0; index < last; index++) {
		cell = getCellN(index);
		v = cell->demiVal;
		if (v) {
			setCell(getCol(index), getRow(index), v, 0);
			printf("Cell <%d,%d> set to %d\n", getCol(index) + 1, getRow(index) + 1, v);	}
	}
	cell = getCellN(last);
	setCell(getCol(index), getRow(index), cell->demiVal, 1);
	if (myGame.mode == Init) {
		return;
	}
	printf("Cell <%d,%d> set to %d\n", getCol(index) + 1, getRow(index) + 1, cell->demiVal);
	printgb();
}

/*
 * reset:
 * ------
 * arguments:
 * none.
 * description:
 * undoes all the moves done by the user, and clears the moves-list.
 */
void reset(void) {

	while (myGame.currMove != myGame.sentinel) {
		undoAny();
	}

	clearNextMoves();

	printf("Board reset\n");

}

/*
 * exitGame:
 * ---------
 * arguments:
 * none.
 * description:
 * a method handling the user-command 'exit', or EOF.
 * frees all memory, prints an exit message to the user,
 * and closes the game.
 */
void exitGame(void) {

	/*free all memory*/

	while (myGame.currMove != myGame.sentinel) {
		undoAny();
	}
	clearNextMoves();
	free(myGame.sentinel);
	free(myGame.board);

	/*print & exit*/

	printf("Exiting...\n");

	exit(0);

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    2. GAME FLOW FUNCTION                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * gameLoop:
 * ---------
 * arguments:
 * none.
 * description:
 * a loop that handles the game flow.
 * starts the game.
 * prints game-flow-related messages to the user.
 * while the game is not closed:
 *      reads input from the user and sends it to the
 *      parser (parseGameLoop)
 */
void gameLoop () {

	char input[256];
	int i;

	myGame.mode = Init;
	myGame.markErrors = 1;

	printf("Sudoku\n------\n");

	while (1) {
		printf("Enter your command:\n");
		for (i=0; i<256; i++) {
			input[i] = 0;
		}
		fgets(input, 256, stdin);
		parseGameLoop(input);
	}

}



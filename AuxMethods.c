/*
 * AuxMethods.c
 *
 * the functions in this file were written to support functions in other modules
 * (and in this one), to break down long functions into shorter ones by taking logical
 * units out, and to avoid code repetition by making functions out of frequently recurring
 * chunks of codes.
 * due to the fact that the functions here are made to be available in other modules,
 * they are all public.
 * contents:
 * 1. parser related functions.
 * 2. erroneous cells related functions.
 * 3. saving and loading related functions.
 * 4. cells and indices related functions.
 * 5. moves-list related functions.
 * 6. set related functions.
 * 7. generate related functions.
 * 8. stack related functions.
 * 9. demi-values related functions.
 * 10. autofill related functions.
 * 11. prints related functions.
 * 12. memory allocation related functions.
 *
 */

#include "DataStructures.h"
#include "AuxMethods.h"
#include "Game.h"
#include "Validate.h"
#include <stdlib.h>
#include <stdio.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          1. PARSER                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * isNum:
 * ------
 * arguments:
 * st - string to be evaluated.
 * description:
 * if st represents an integer:
 * 	   returns true.
 * else:
 *     returns false.
 */
int isNum(char *st){
	int i = 0;
	char c = st[i];
	while(c!='\0')
	{
		if (((int)c<48) || ((int)c>57))
			return 0;
		i++;
		c = st[i];
	}
	return 1;
}

/*
 * hasNewLine:
 * -----------
 * arguments:
 * input - string to be evaluated.
 * description:
 * if input[] contains new-line or EOF:
 *     returns true.
 * else:
 *     returns false.
 */
int hasNewLine(char input[]) {
	int counter = 0;
	while (counter < 256) {
		if (input[counter] == 10 || feof(stdin)) {
			return 1;
		}
		counter++;
	}
	return 0;
}

/* parseMarkErrorInput:
 * --------------------
 * arguments:
 * token - string to be parsed.
 * description:
 * an implementation of a deterministic finite automata
 * to determine whether the int value of input is 1 (returns 1),
 * 0 (returns 0) or neither (returns -1).
 */
int parseMarkErrorInput(char * token) {
	int state, mark, i;
	while (1) {
		if (state == 0) { /*state 0 is the starting state*/
			if (token[i] == '0') {
				state = 1;
				i++;
				continue;
			}
			if (token[i] == '1') {
				state = 2;
				i++;
				continue;
			}
			mark = -1;
			break;
		}
		if (state == 1) { /*state 1 means input so far is "0*" */
			if (token[i] == '0') {
				i++;
				continue;
			}
			if (token[i] == '1') {
				state = 2;
				i++;
				continue;
			}
			if (token[i] == 0) {
				mark = 0;
				break;
			}
			mark = -1;
			break;
		}
		if (state == 2) { /*state 2 means input so far is "0*1" */
			if (token[i] == 0) {
				mark = 1;
				break;
			}
			mark = -1;
			break;
		}
	}
	return mark;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                      2. ERRONEOUS CELLS                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * checkIfError:
 * -------------
 * arguments:
 * col - column no. of the evaluated cell.
 * row - row no. of the evaluated cell.
 * val - new value of the evaluated cell.
 * old - old value of the evaluated cell.
 * description:
 * checks whether assigning the new value 'val' to cell <col,row>
 * requires updating the isError status of cell <col,row>.
 * additionally, calls isErrorUpdate in order to update
 * all cells affected by the changing of cell <col,row>'s
 * value to 'val.
 */
void checkIfError(int col, int row, int val, int old){

	int i,j,xx,yy, f=0;

	/*if val==0 dont mark errors, but check whether other cells are still erroneous*/
	if (val == 0) {
		if ((getCell(col,row)->isError == 1) && (!f) && (val!=old)) /* val(col,row) was changed, we have to check if it's neighbors are still erroneous*/
			isErrorUpdate(col, row, old);
		getCell(col,row)->isError = 0;
		return;
	}

	/*check whether this val already exists in col*/
	for (i=0;i<myGame.N; i++){
		if (i == row) {
			continue;
		}
		if (getCell(col,i)->val == val){
			f=1;
			getCell(col,i)->isError = 1;}
	}

	/*check whther this val already exists in row*/
	for (i=0; i<myGame.N; i++){
		if (i==col) {
			continue;
		}
		if (getCell(i,row)->val == val){
			f=1;
			getCell(i,row)->isError = 1;}
	}

	/*check whther this val already exists in this block*/
	xx = col - col%myGame.blockW;
	yy = row - row%myGame.blockH;
	for (i=0; i <myGame.blockW ; i++){
		for (j=0; j < myGame.blockH ; j ++){
			if ((getCell(xx+i,yy+j)->val ==val) && (((xx+i)!=col) || ((yy+j)!=row))){
				f=1;
				getCell(xx+i,yy+j)->isError = 1;}
		}
	}

	/* val(col,row) was changed, we have to check if it's neighbors are still erroneous*/
	if ((getCell(col,row)->isError == 1)  && (val!=old))
		isErrorUpdate(col, row, old);

	/*update cell(col,row) isError value*/
	getCell(col,row)->isError = f;

}

/*
 * isErrorUpdate:
 * --------------
 * arguments:
 * col - column of the updated cell.
 * row - row of the updated cell.
 * old - old value of the updated cell.
 * description:
 * updates the isError value of cell <col,row>.
 * Additionally, calls checkIfError for neighboring
 * cells, if necessary.
 */
void isErrorUpdate(int col, int row, int old){
	int i, j, xx, yy;

	/*check whether old val exists in col*/
	for (i=0; i<myGame.N; i++){
		if (i==row) {
			continue;
		}
		if (getCell(col,i)->val == old){
			checkIfError(col, i, old, old);
		}
	}

	/*check whether old val exists in row*/
	for (i=0; (i<myGame.N); i++){
		if (i == col) {
			continue;
		}
		if (getCell(i,row)->val == old){
			checkIfError(i ,row, old, old);
		}
	}

	/*check whether this val already exists in this block*/
	xx = col - col%myGame.blockW;
	yy = row - row%myGame.blockH;
	for (i=0; i <myGame.blockW ; i++){
		for (j=0; j < myGame.blockH ; j ++){
			if ((getCell(xx+i,yy+j)->val ==old) && (((xx+i)!=col) || ((yy+j)!=row))){
				checkIfError(xx+i,yy+j, old, old);}
		}
	}
}

/*
 * isErroneus:
 * -----------
 * arguments:
 * none.
 * description:
 * if board contains erroneus values:
 *     returns true.
 * else:
 *     returns false.
 */
int isErroneous(){
	int i,j;
	for (i=0; i<myGame.N;i++){
		for(j=0; j<myGame.N;j++){
			if (getCell(i,j)->isError)
				return 1;
		}
	}
	return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                      3. SAVING AND LOADING                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * readSize:
 * ---------
 * arguments:
 * ifp - pointer to a FILE to read from.
 * description:
 * reads the next 1-5 integer from ifp,
 * returns it in int format.
 */
int readSize(FILE *ifp){
	char c = fgetc(ifp);
	while (c!='1' && c!='2' && c!='3' && c!='4' && c!='5')
		c = fgetc(ifp);
	return (c - 48);
}

/*
 * readNextCell:
 * -------------
 * arguments:
 * ifp - pointer to a FILE to read from.
 * cell - pointer to a cell to parse the data from the file into.
 * description:
 * reads the next integer representing a cell from 'ifp',
 * and fills 'cell' accordingly.
 */
void readNextCell(FILE *ifp, Cell *cell) {
	char c;
	int val, fixed = 0;

	/*skip over whitespaces (since saved files are guaranteed to be valid, we can assume non-integer chars are whitespace).*/
	c=fgetc(ifp);
	while ( c!='0' && c!='1' && c!='2' && c!='3' && c!='4' && c!='5' && c!='6' && c!='7' && c!='8' && c!='9') {
		c=fgetc(ifp);
	}

	/*parse an int*/
	val = c - 48;
	c=fgetc(ifp);
	if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
		val = 10*val + (c - 48);
		c=fgetc(ifp);
	}

	/*parse cell's 'fixed' status*/
	if ( c == '.') {
		fixed = 1;
	}
	cell->val = val;
	cell->isFixed = fixed;

	/*update the game parameters*/
	if (val) {
		myGame.numFilled++;
	}
}

/*
 * writeSize:
 * ----------
 * arguments:
 * ifp - pointer to a FILE to write to.
 * height - the number of rows in a block.
 * width - the number of columns in a block.
 * description:
 * writes the dimensions (m,n) of the board
 * into ifp.
 */
int writeSize(FILE *ifp, int height, int width){
	int f;
	char m1,n1;
	m1 = height + 48;
	n1 = width + 48;
	f = fputc(m1,ifp);
	if (!f)
		return 0;
	f = fputc(' ',ifp);
	if (!f)
		return 0;
	f = fputc(n1,ifp);
	if (!f)
		return 0;
	f = fputc('\n',ifp);
	if (!f)
		return 0;
	return 1;
}

/*
 * writeCell:
 * ----------
 * arguments:
 * ifp - pointer to a FILE to write into.
 * cell - pointer to the cell to be saved.
 * val - the value of the saved cell.
 * description:
 * saves 'cell' into ifp according the saving format.
 * if successful:
 *     returns true.
 * else:
 *     returns false.
 */
int writeCell(FILE *ifp, Cell *cell, int val){
	int v1,f;
	if (val<=9){ /*case where the value is single-digit*/
		v1 = val + 48;
		f = fputc(v1, ifp);
		if (!f)
			return 0;
	}
	else{ /*case where the value is double-digit*/
		v1 = (val/10) + 48;
		f = fputc(v1,ifp);
		if (!f)
			return 0;
		v1 = (val%10) + 48;
		f = fputc(v1,ifp);
		if (!f)
			return 0;
	}

	/*write a '.' if cell should be considered fixed.*/
	if (((myGame.mode == Edit) && (val!=0)) || (cell->isFixed)){
		f = fputc('.',ifp);
		if (!f)
			return 0;}

	f = fputc(' ', ifp);

	if (!f)
		return 0;
	return 1;
}

/*
 * writeCellEndOfLine:
 * -------------------
 * arguments:
 * ifp - pointer to the FILE that is to be written into.
 * cell - pointer to the cell to be saved.
 * val - the value of the saved cell.
 * description:
 * saves 'cell' into ifp according the saving format,
 * when 'cell' is in the end of a line.
 * if successful:
 *     returns true.
 * else:
 *     returns false.
 */
int writeCellEndOfLine(FILE *ifp, Cell *cell, int val){
	int v1,f;
	if (val<=9){ /*case where the value is single-digit*/
		v1 = val + 48;
		f = fputc(v1, ifp);
		if (!f)
			return 0;}
	else{ /*case where the value is double-digit*/
		v1 = (val/10) + 48;
		f = fputc(v1,ifp);
		if (!f)
			return 0;
		v1 = (val%10) + 48;
		f = fputc(v1,ifp);
		if (!f)
			return 0;}

	/*write a '.' if cell should be considered fixed.*/
	if (((myGame.mode == Edit) && (val!=0)) || (cell->isFixed)){
		f = fputc('.',ifp);
		if (!f)
			return 0;}

	if (!f)
		return 0;

	return 1;
}

/*
 * createGame:
 * -----------
 * arguments:
 * blockW - block width.
 * blockH - block height.
 * m - game mode.
 * description:
 * initializes the board and all other myGame parameters in
 * preparation for a new game, according to the block-height,
 * block-width and game-mode provided.
 */
void createGame(int blockW, int blockH, enum Mode m) {

	/*if not the first time initialized, clear old memory*/
	if (myGame.notNew) {
		while (myGame.currMove != myGame.sentinel) {
			undoAny();
		}
		clearNextMoves();
		free(myGame.sentinel);
		free(myGame.board);

	}

	/*allocate new memory*/
	myGame.sentinel = createMove(0,0,0,0,0);
	myGame.currMove = myGame.sentinel;
	myGame.numFilled = 0;
	myGame.blockH = blockH;
	myGame.blockW = blockW;
	myGame.N = blockH*blockW;
	myGame.mode = m;
	myGame.board = calloc(myGame.N*myGame.N, sizeof(Cell));
	myGame.notNew = 1;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                      4. CELLS AND INDICES                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * getRow:
 * -------
 * arguments:
 * n - cell's number (counting left to right, then top to bottom).
 * description:
 * returns the number of the row of the n'th cell
 * in the board.
 */
int getRow(int n) {
	return n/myGame.N;
}

/*
 * getCol:
 * -------
 * arguments:
 * n - cell's number (counting left to right, then top to bottom).
 * description:
 * returns the number of the column of the n'th cell
 * in the board.
 */
int getCol(int n) {
	return n%myGame.N;
}

/*
 * getN:
 * -----
 * arguments:
 * col - cell's column.
 * row - cell's row.
 * description:
 * returns the number (when counting left to right,
 * then top to bottom) of the cell in position <col,row>.
 */
int getN(int col, int row) {
	return row*myGame.N + col;
}

/*
 * getCell:
 * --------
 * arguments:
 * col - cell's column.
 * row - cell's row.
 * description:
 * returns a pointer to the cell in position <col,row>.
 */
Cell* getCell(int col, int row) {
	return myGame.board + getN(col, row);
}

/*
 * getCellN:
 * ---------
 * arguments:
 * n - cell's number (counting left to right, then top to bottom).
 * description:
 * returns a pointer to the n'th cell in the board.
 */
Cell* getCellN(int n) {
	return myGame.board + n;
}

/*
 * nextEmptyCell:
 * --------------
 * arguments:
 * i - current cell's number (counting left to right, then top to bottom).
 * description:
 * returns the index of the next empty cell after cell number i.
 * if there is no such cell, returns N*N.
 */
int nextEmptyCell(int i){
	if (i >= myGame.N*myGame.N) {
		return i;
	}
	while (getCellN(i)->val) {
		i++;
		if (i >= myGame.N*myGame.N) {
			return i;
		}
	}
	return i;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          5. MOVES-LIST                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * createMove:
 * -----------
 * arguments:
 * col - cell's column.
 * row - cell's row.
 * oldVal - cell's old value.
 * newVal - cell's new value.
 * user - indicates whether the move was user-made or automatic.
 * description:
 * returns a pointer to a new MovesList instance.
 */
MovesList* createMove(int col, int row, int oldVal, int newVal, int user) {
	MovesList *newMove = safealloc(sizeof(MovesList));
	newMove->row = row;
	newMove->col = col;
	newMove->oldVal = oldVal;
	newMove->newVal = newVal;
	newMove->user = user;
	newMove->next = NULL;
	newMove->prev = NULL;
	return newMove;
}

/*
 * addMove:
 * --------
 * arguments:
 * col - cell's column.
 * row - cell's row.
 * oldVal - cell's old value.
 * newVal - cell's new value.
 * user - indicates whether the move was user-made or automatic.
 * description:
 * adds a move to the moves-list, after one marked as current,
 * and updates the list accordingly.
 */
void addMove(int col, int row, int oldVal, int newVal, int user) {
	MovesList *newMove = createMove(col, row, oldVal, newVal, user);
	clearNextMoves();
	/*if lastmove isnt null, connect pointers from lastmove to newmove*/
	if (myGame.currMove != NULL) {
		myGame.currMove->next = newMove;
		newMove->prev = myGame.currMove;
	}
	/*make newmove the last move*/
	myGame.currMove = newMove;
}

/*
 * clearNextMoves:
 * ---------------
 * arguments:
 * none.
 * description:
 * clears all the moves after the current move in the moves-list.
 */
void clearNextMoves(void) {
	MovesList *pcurr, *pnext;
	/*case: there are no moves*/
	if ( myGame.currMove == NULL) return;
	/*case: current move is the last move*/
	pcurr = myGame.currMove->next;
	if (pcurr==NULL) {
		myGame.currMove->next = NULL;
		return;
	}
	/*case: current move is the move before the last*/
	pnext = pcurr->next;
	if (pnext == NULL) {
		free(pcurr);
		myGame.currMove->next = NULL;
		return;
	}
	/*case: there are 2 or more moves after currMove*/
	while (pnext->next != NULL) {
		free(pcurr);
		pcurr = pnext;
		pnext = pcurr->next;
	}
	free(pcurr);
	free(pnext);
	myGame.currMove->next = NULL;
}

/*
 * undoAny:
 * --------
 * arguments:
 * none.
 * description:
 * reverts the current move in moves-list,
 * and updates the board and moves-list accordingly.
 */
void undoAny(void) {
	MovesList m = *myGame.currMove;
	Cell *c = getCell(m.col, m.row);
	c->val = m.oldVal;
	myGame.currMove = myGame.currMove->prev;
	if (!(m.oldVal) && (m.newVal)) {
		myGame.numFilled--;
	}
	if ((m.oldVal) && !(m.newVal)) {
		myGame.numFilled++;
	}
	checkIfError(m.col, m.row, m.oldVal, m.newVal);
}

/*
 * redoAny:
 * --------
 * arguments:
 * none.
 * description:
 * redoes a move.
 * updates the board and moveslist accordingly.
 */
void redoAny(void) {
	MovesList m;
	Cell *c;
	myGame.currMove = myGame.currMove->next;
	m = *myGame.currMove;
	c = getCell(m.col, m.row);
	c->val = m.newVal;
	if (!(m.oldVal) && (m.newVal)) {
		myGame.numFilled++;
	}
	if ((m.oldVal) && !(m.newVal)) {
		myGame.numFilled--;
	}
	checkIfError(m.col, m.row, m.newVal, m.oldVal);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             6. SET                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * setCell:
 * --------
 * arguments:
 * col - cell's column.
 * row - cell's row.
 * newVal - cell's new value.
 * user - indicates whether the move was user-made or automatic.
 * description:
 * sets cell <col,row> to value newVal, and marks it as
 * user-made or automatic move according to 'user'.
 * updates the moves-list, and the board.
 */
void setCell(int col, int row, int newVal, int user) {
	int oldVal;
	Cell *pcell;

	pcell = getCell(col, row);
	oldVal = pcell->val;
	pcell->val = newVal;
	addMove(col, row, oldVal, newVal, user);

	/*update numFilled*/
	if ((oldVal == 0) && (newVal != 0))
		myGame.numFilled++;
	if ((oldVal != 0) && (newVal == 0))
		myGame.numFilled--;

	checkIfError(col, row, newVal, oldVal);

	if ((myGame.mode == Solve) && (myGame.numFilled == myGame.N*myGame.N)) {/*if this is the last cell to be filled*/
		printgb();
		if (!ILPvalidate()) /*validation failed*/
			printf("Puzzle solution erroneous\n");
		else{
			printf("Puzzle solved successfully\n");
			myGame.mode = Init;
		}
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           7. GENERATE                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * randInt:
 * --------
 * arguments:
 * range - the range from which the random int will be chosen.
 * description:
 * returns a random integer between 0-(range-1).
 */
int randInt(int range) {
	return rand()%range;
}

/*
 * clearBoard:
 * -----------
 * arguments:
 * none.
 * description:
 * clears the values of every cell in the board.
 */
void clearBoard() {
	int size = myGame.N*myGame.N, i=0;
	Cell *cell;
	for (;i<size; i++) {
		cell = getCellN(i);
		cell->val = 0;
	}
}

/*
 * tryGenerate:
 * ------------
 * arguments:
 * x - number of cells to be randomly filled.
 * y - number of cells not to be erased.
 * description:
 * attempts to fill X cells with random legal values, and then
 * if the board is valid, erases all but Y cells.
 * if successful:
 *     returns true.
 * else:
 *     returns false
 */
int tryGenerate(int x, int y) {
	int size = myGame.N*myGame.N, counter = 0, n = 0;
	Cell *cell;
	/*fill X cells with random legal values:*/
	while (counter < x) {
		n = randInt(size);
		cell = getCellN(n);
		if (cell->val !=0) continue;
		n = randInt(myGame.N) + 1;
		cell->val = n;
		counter++;
	}

	/*if board isnt valid, return 0*/
	if (!ILPvalidate()) {
		clearBoard();
		return 0;
	}
	/*copy all demiVals to vals, so the board is solved*/
	copyDemivalsToVals();
	/*if board is valid, choose Y cells to preserve :*/
	clearDemiVals(0);
	counter = 0;
	while (counter < y) {
		cell = getCellN(randInt(size));
		if (cell->demiVal == 0) {
			cell->demiVal = cell->val + 26; /*we add 26 to val, so we would later be able to recognize 0-valued cells too*/
			counter++;
		}
	}

	/*clear all cells*/
	clearBoard();

	/*set the Y cells that were chosen:*/
	counter = 0;
	n = 0;
	while (counter < y) { /*set the y-1 first cells*/
		cell = getCellN(n);
		if (cell->demiVal != 0) {
			if (cell->demiVal != 26) setCell(getCol(n), getRow(n), cell->demiVal - 26, 0);
			cell->demiVal = 0;
			counter++;
		}
		n++;
	}
	myGame.currMove->user = 1;

	return 1;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            8. STACK                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * createElem:
 * -----------
 * arguments:
 * cellIndex = the requested cellIndex value for the new Elem.
 * description:
 * returns a pointer to a new Elem
 */
Elem *createElem(int cellIndex){
	Elem *e = safealloc(sizeof(Elem));
	if (!e)
		return NULL;
	e->cellIndex = cellIndex;
	return e;
}

/*
 * push:
 * -----
 * arguments:
 * st - the stack to push into
 * cellIndex - the desired cellIndex value for the new Elem to be pushed into the stack.
 * description:
 * attempts to push the stack st with a new Elem
 * containing the value cellIndex.
 * returns 1 iff new Elem was created and pushed successfully.
 */
int push (Stack *st, int cellIndex){
	Elem *e = createElem(cellIndex);
	if (!e)
		return 0;
	e->next = st->top;
	st->top = e;
	st->numOfElements++ ;
	return 1;
}

/*
 * pop:
 * ----
 * arguments:
 * st - the stack to be popped.
 * description:
 * removes the top of stack st,
 * and returns a pointer to it.
 */
Elem *pop(Stack *st){
	Elem *e;
	if (!(st->numOfElements))
		return NULL;
	st->numOfElements--;
	e = st->top;
	st->top = e->next;
	return e;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         9. DEMI-VALUES                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * clearDemiVals:
 * --------------
 * arguments:
 * i - the number (left to right, then top to bottom) of the cell that all
 * cells with number greater than it, will be cleared.
 * description:
 * clears the demiVals of all cells with index
 * greater than i.
 */
void clearDemiVals(int i) {

	for (; i < myGame.N*myGame.N; i++) {
		getCellN(i)->demiVal = 0;
	}

}

/*
 * copyValsToDemivals:
 * -------------------
 * arguments:
 * none.
 * description:
 * for every cell in board:
 *     val = demiVal.
 */
void copyValsToDemivals() {

	int i;

	for (i = 0; i < myGame.N*myGame.N; i++) {
		if (getCellN(i)->val) {
			getCellN(i)->demiVal = getCellN(i)->val;
		}
	}

}

/*
 * copyDemivalsToVals:
 * -------------------
 * arguments:
 * none.
 * description:
 * for every cell in board:
 *     demiVal = val.
 */
void copyDemivalsToVals() {

	int i;

	for (i = 0; i < myGame.N*myGame.N; i++) {
		if (getCellN(i)->demiVal) {
			getCellN(i)->val = getCellN(i)->demiVal;
		}
	}

}

/*
 * isDemiValid:
 * ------------
 * arguments:
 * n - the number (left to right, then top to bottom) of the cell to be checked.
 * description:
 * returns 1 iff the demiVal of the n'th cell
 * is valid with regard to the demi-values of other cells.
 */
int isDemiValid(int n) {

	int demiVal = getCellN(n)->demiVal;
	int col = getCol(n);
	int row = getRow(n);
	int N = myGame.N;
	int bh = myGame.blockH;
	int bw = myGame.blockW;
	int i, j;

	/*check col*/

	for (i=0; i<N; i++) {
		if (i == row) {
			continue;
		}
		if (getCell(col, i)->demiVal == demiVal) {
			return 0;
		}
	}

	/*check row*/

	for (i=0; i<N; i++) {
		if (i == col) {
			continue;
		}
		if (getCell(i, row)->demiVal == demiVal) {
			return 0;
		}
	}

	/*check block*/

	for (i = col - col%bw; i < col - col%bw + bw; i++) {
		for (j = row - row%bh; j < row - row%bh + bh; j++) {
			if (i == col && j == row) {
				continue;
			}
			if (getCell(i, j)->demiVal == demiVal) {
				return 0;
			}
		}
	}


	return 1;

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          10. AUTOFILL                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * getCellSingleValue:
 * -------------------
 * arguments:
 * index - the number (left to right, then top to bottom) of the cell to be evaluated.
 * description:
 * if there is a single legal value for the n'th cell:
 *     return that value.
 * else:
 *     return 0.
 */
int getCellSingleValue(int index) {
	int col = getCol(index);
	int row = getRow(index);
	int N = myGame.N;
	int bh = myGame.blockH;
	int bw = myGame.blockW;
	int val;
	int counter = 0;
	int result;
	int i, j;

	/*count no. of legal vals:*/

	for (val = 1; val <= N; val++) {

		/*check col*/

		for (i=0; i<N; i++) {
			if (i == row) {
				continue;
			}
			if (getCell(col, i)->val == val) {
				goto INVALID;
			}
		}

		/*check row*/

		for (i=0; i<N; i++) {
			if (i == col) {
				continue;
			}
			if (getCell(i, row)->val == val) {
				goto INVALID;
			}
		}

		/*check block*/

		for (i = col - col%bw; i < col - col%bw + bw; i++) {
			for (j = row - row%bh; j < row - row%bh + bh; j++) {
				if (i == col && j == row) {
					continue;
				}
				if (getCell(i, j)->val == val) {
					goto INVALID;
				}
			}
		}

		counter++;
		result = val;

		INVALID:
		continue;}
	/*if only 1 legal value, return it. else return 0 */
	if (counter == 1) {
		return result;}
	return 0;}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           11. PRINTS                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * printUndo:
 * ----------
 * arguments:
 * move - the move to be printed.
 * description:
 * prints a message to the user, following an undo command.
 */
void printUndo (MovesList *move) {
	char x, y, z1, z2;
	x = move->col + 49;
	y = move->row + 49;
	z1 = move->newVal + 48;
	z2 = move->oldVal + 48;
	if (z1 == '0') z1 = '_';
	if (z2 == '0') z2 = '_';
	printf("Undo %c,%c: from %c to %c\n", x, y, z1, z2);
}

/*
 * printRedo:
 * ----------
 * arguments:
 * move - the move to be printed.
 * description:
 * prints a message to the user, following an redo command.
 */
void printRedo (MovesList *move) {
	char x, y, z1, z2;
	x = move->col + 49;
	y = move->row + 49;
	z1 = move->oldVal + 48;
	z2 = move->newVal + 48;
	if (z1 == '0') z1 = '_';
	if (z2 == '0') z2 = '_';
	printf("Redo %c,%c: from %c to %c\n", x, y, z1, z2);
}

/*
 * printCell:
 * ----------
 * arguments:
 * x - column of the cell
 * y - row of the cell.
 * description:
 * prints the cell <x,y> in the requested format
 */
void printCell(int x, int y) {
	int f = 0;
	Cell * pcell;
	pcell = myGame.board + getN(x, y);
	printf(" ");
	if (pcell->val == 0) printf("  ");
	else printf("%2d", pcell->val);
	if (pcell->isFixed == 1) {
		printf(".");
		f = 1;
	}
	else{
		if ((myGame.mode ==  Edit) || myGame.markErrors == 1){
			if (pcell->isError == 1) {
				printf("*");
				f = 1;
			}
		}
	}
	if (f==0) printf(" ");

}

/*
 * printSeparatorRow:
 * ------------------
 * arguments:
 * none.
 * description:
 * prints a separator row in the requested format.
 */
void printSeparatorRow () {
	int i, n = myGame.blockH, m = myGame.blockW;
	for (i=0; i<(4*n*m + n); i++)
		printf("-");
	printf("-\n");
}

/*
 * printRow:
 * ---------
 * arguments:
 * r - number of the row.
 * description:
 * prints a row (filled with cells) in the requested format.
 */
void printRow(int r) {
	int j;
	for (j=0; j<myGame.N; j++){
		if ((j%myGame.blockW) == 0)
			printf("|");
		printCell(j,r);
	}
	printf("|\n");
}

/*
 * printgb:
 * ---------
 * arguments:
 * none.
 * description:
 * prints the game board according to the requested print format.
 */
void printgb() {
	int i;
	for (i=0; i<myGame.N; i++){
		if ((i%myGame.blockH) == 0)
			printSeparatorRow();
		printRow(i);
	}
	printSeparatorRow();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                      12. MEMORY ALLOCATION                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * safealloc:
 * ---------
 * arguments:
 * size - size of the memory to be allocated.
 * description:
 * attempts to allocate memory of the requested size.
 * if allocation was successful, return a pointer to the memory address.
 * if allocation fails, tries again. if failed twice,
 * prints an error message and closes the program.
 */
void * safealloc(int size) {
	void *p = malloc(size);
	if (p == NULL) {
		p = malloc(size);
		if (p==NULL) {
			printf("Fatal error: memory allocation failed\n");
			exitGame();
		}
	}
	return p;
}

/*
 * AuxMethods.h
 *
 * this file allows other modules to use the functions
 * from AuxMethods.c.
 *
 */

#ifndef AUXMETHODS_H_
#define AUXMETHODS_H_

/*to avoid compilation problems caused by the types FILE, Cell, etc...*/
#include <stdio.h>
#include "DataStructures.h"

int isNum(char *st);
void checkIfError(int col, int row, int val, int old);
void isErrorUpdate(int col,int row,int old);
int isErroneous();
int readSize(FILE *ifp);
void readNextCell(FILE *ifp, Cell *cell);
int writeSize(FILE *ifp, int height, int width);
int writeCell(FILE *ifp, Cell *cell, int val);
int getRow(int n);
int getCol(int n);
int getN(int col, int row);
Cell* getCell(int col, int row);
Cell* getCellN(int n);
void setCell(int col, int row, int newVal, int user);
void addMove(int col, int row, int oldVal, int newVal, int user);
void printMovesList(void);
void clearNextMoves(void);
void createMovesList1(void);
void createGame(int blockW, int blockH, enum Mode m);
MovesList* createMove(int col, int row, int oldVal, int newVal, int user);
void undoAny(void);
void redoAny(void);
int randInt(int range);
int tryGenerate(int x, int y);
void clearDemiVals(int i);
int isDemiValid(int n);
void copyValsToDemivals();
void copyDemivalsToVals();
int getCellSingleValue(int index);
void printDemiBoard();
void printStack(Elem *e, Stack *st);
void printUndo (MovesList *move);
void printRedo (MovesList *move);
Elem *createElem(int cellIndex);
int push (Stack *st, int cellIndex);
Elem *pop(Stack *st);
int writeCellEndOfLine(FILE *ifp, Cell *cell, int val);
int hasNewLine(char input[]);
int nextEmptyCell(int i);
void printCell(int x, int y);
void printSeparatorRow (void);
void printRow(int r);
void printgb(void);
int parseMarkErrorInput(char * token);
void * safealloc(int size);

#endif /* AUXMETHODS_H_ */





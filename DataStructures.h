/*
 * DataStructures.h
 *
 * this file contains the data structures that will be used
 * throughout the code.
 *
 */

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_


/*
 * Cell:
 * -----
 * implementation of a board cell.
 */
struct Cell
{
	int
		isFixed,
		isError,
		val,
		demiVal; /*this field will be used when we want to store a value without changing the original*/
} ;
typedef struct Cell Cell;

/*
 * MovesList:
 * ----------
 * implementation of a move in the moves-list.
 */
struct MovesList
{
	int
		row,
		col,
		oldVal,
		newVal,
		user; /*this field indicates whether the was user made or automatic (as in generate and autofill).*/
	struct MovesList
		*next,
		*prev;
} ;
typedef struct MovesList MovesList;

/*
 * Mode:
 * -----
 * an enum to represent the 3 game modes.
 */
enum Mode{
	Init,
	Solve,
	Edit
};

/*
 * Game:
 * -----
 * a struct that stores all game parameters and relevant pointers.
 */
typedef struct
{
	int
		blockH,
		blockW,
		N, /*size of a row/column*/
		markErrors,
		numFilled,
		notNew; /*indicates whether memory was allocated to board, sentinel and currMoves*/
	enum Mode
		mode;
	MovesList
		*sentinel, /*a MovesList variable indicating the beggining of the moves-list*/
		*currMove; /* a pointer to the current move. */
	Cell
		*board;
} Game;
extern Game myGame; /* myGame is the Game-type-variable that will be
                     * be referred to throughout the code.*/

/*
 * Elem:
 * -----
 * an implementation of a stack element.
 */
struct Elem
{
	int	cellIndex;
	MovesList *move; /* to be used in the prints accompanying undo and redo.*/
	struct Elem *next;
} ;
typedef struct Elem Elem;

/*
 * Stack:
 * ------
 * an implementation of a stack.
 * to be used in: num_solutions,
 * undo and redo.
 */
struct Stack
{
	int numOfElements;
	struct Elem *top;
};
typedef struct Stack Stack;


#endif /* DATASTRUCTURES_H_ */

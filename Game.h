/*
 * Game.h
 *
 * this file allows other modules to use the functions in game.c.
 */

#ifndef GAME_H_
#define GAME_H_

void solve(char *file_name);
void edit(void);
void edit1(char *file_name);
void set(int n1,int n2,int n3);
void validate(void);
void generate(int x,int y);
void undo(void);
void redo(void);
void save(char* address);
void hint(int col, int row);
void numSolutions(void);
void autofill(void);
void reset(void);
void exitGame(void);
void gameLoop ();


#endif /* GAME_H_ */

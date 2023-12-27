/*
 * main.c
 *
 * this file contains our main function, which sets the random seed and
 * calls the function that starts the game.
 *
 */


#include "DataStructures.h"
#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
 * main:
 * -----
 * arguments:
 * none.
 * description:
 * sets the random seed and
 * calls the function that starts the game.
 */
int main(){
	srand(time(NULL));
	gameLoop();
	return 0;
}

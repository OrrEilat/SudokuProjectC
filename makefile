CC = gcc
OBJS = main.o DataStructures.o AuxMethods.o Validate.o Game.o Parser.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56


$(EXEC): main.o 
	gcc -o sudoku-console main.o Parser.o Game.o Validate.o AuxMethods.o DataStructures.o $(GUROBI_LIB)

all : sudoku-console

DataStructures.o: DataStructures.h
	$(CC) $(COMP_FLAG) -c $*.c

AuxMethods.o: AuxMethods.h DataStructures.o
	$(CC) $(COMP_FLAG) -c $*.c

Validate.o: Validate.h AuxMethods.o
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c

Game.o: Game.h Validate.o
	$(CC) $(COMP_FLAG) -c $*.c
	
Parser.o: Parser.h Game.o
	$(CC) $(COMP_FLAG) -c $*.c
		
main.o: Parser.o
	$(CC) $(COMP_FLAG) -c $*.c
	
clean:
	rm -f $(OBJS) $(EXEC)

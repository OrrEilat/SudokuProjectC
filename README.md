# SudokuProjectC
A command-line sudoku program in C, written as part of the course "Software Project" in Tel Aviv University.

The program's features include:
- A menu
- Generating sudoku boards of various sizes
- Enabling the user to play sudoku
- Providing hints, and solving boards
- Saving and loading

The logic of solving a sudoku board is implemented by representing the board as an Integer Linear Programming problem, and then using the Gurobi library to solve that problem.

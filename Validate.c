/*
 * Validate.c
 *
 * this file implements the ILP-solve algorithm using the Gurobi library.
 * contents:
 * 1. auxiliary functions that support our ILP-solve function. (private)
 * 2. ILP-solve function - ILPvalidate. (public)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "DataStructures.h"
#include "AuxMethods.h"
#include "gurobi_c.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    1. AUXILIARY FUNCTIONS                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * storeSolution:
 * --------------
 * arguments:
 * model - gurobi model.
 * solution - an array to extract the solution into.
 * description:
 * extract a solution from 'model', and stores it in
 * each cell's demiVal field.
 */
int storeSolution (GRBmodel *model, double *solution) {
	int error, v,i,j,N = myGame.N;
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N*N*N, solution); /*solution extracted from model to 'solution'*/
	if (error) return error;

	/*storing the solution in each cell's demiVal field*/
	for (v=0; v<N; v++) {
		for (i=0; i<N; i++) {
			for (j=0; j<N; j++) {
				if (*(solution + N*N*i + N*j + v) == 1.0) {
					getCell(i,j)->demiVal = v + 1;
				}
			}
		}
	}
	return error;
}

/*
 * addBlockConstraints:
 * --------------------
 * arguments:
 * ind - an array of variable indices.
 * val - an array of variable values.
 * model - a gurobi model.
 * description:
 * adds the constraints, regarding board-blocks, imposed by the
 * current game state, to 'model'.
 */
int addBlockConstraints(int *ind, double *val, GRBmodel *model) {
	int error, count, v,a,b,i,j,N = myGame.N;
	for (v=0; v<N; v++) {
		for (a=0; a<myGame.blockH; a++) {
			for (b=0; b<myGame.blockW; b++) {
				count = 0;
				for (i=a*myGame.blockW; i < (a+1)*myGame.blockW; i++) {
					for (j=b*myGame.blockH; j < (b+1)*myGame.blockH; j++) {
						ind[count] = N*N*i + N*j + v;
						val[0] = 1.0;
						count++;
					}
				}
				error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) return error;
			}
		}
	}
	return error;
}

/*
 * addRowConstraints:
 * ------------------
 * arguments:
 * ind - an array of variable indices.
 * val - an array of variable values.
 * model - a gurobi model.
 * description:
 * adds the constraints, regarding board-rows, imposed by the
 * current game state, to 'model'.
 */
int addRowConstraints (int *ind, double *val, GRBmodel *model) {
	int error, v, i, j, N = myGame.N;
	for (v=0; v<N; v++) {
		for (j=0; j<N; j++) {
			for (i=0; i<N; i++) {
				ind[i] = N*N*i + N*j + v;
				val[i] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) return error;
		}
	}
	return error;
}

/*
 * addColsConstraints:
 * -------------------
 * arguments:
 * ind - an array of variable indices.
 * val - an array of variable values.
 * model - a gurobi model.
 * description:
 * adds the constraints, regarding board-columns, imposed by the
 * current game state, to 'model'.
 */
int addColsConstraints (int *ind, double *val, GRBmodel *model) {
	int error, i, j, v, N = myGame.N;
	for (v=0; v<N; v++) {
		for (i=0; i<N; i++) {
			for (j=0; j<N; j++) {
				ind[j] = N*N*i + N*j + v;
				val[j] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) return error;
		}
	}
	return error;
}

/*
 * addCellSingleValueConstraint:
 * -----------------------------
 * arguments:
 * ind - an array of variable indices.
 * val - an array of variable values.
 * model - a gurobi model.
 * description:
 * adds to 'model' the constraints imposed by the fact that every
 * cells must hold a single value and only a single value.
 */
int addCellSingleValueConstraint (int *ind, double *val, GRBmodel *model) {
	int error, i, j, v, N = myGame.N;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			for (v=0; v<N; v++) {
				ind[v] = N*N*i + N*j + v;
				val[v] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL , 1.0, NULL);
			if (error) return error;
		}
	}
	return error;
}

/*
 * prepareTypesAndBounds:
 * ----------------------
 * arguments:
 * vtype - an array of gurobi variable types.
 * lb - an array of variables' lower bounds.
 * description:
 * prepares arrays representing variable types (vtype),
 * and variable lower bounds (lb) to be used as arguments
 * when calling GRBnewmodel().
 */
void prepareTypesAndBounds(char *vtype, double *lb) {
	int i, j, v, N = myGame.N;
	Cell *cell;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			for (v=0; v<N; v++) {
				*(vtype + N*N*i + N*j + v) = GRB_BINARY;
				cell = getCell(i,j);
				if (cell->val == v + 1) {
					*(lb + N*N*i + N*j + v) = 1.0;
				} else {
					*(lb + N*N*i + N*j + v) = 0.0;
				}
			}
		}
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    2. ILP SOLVE FUNCTION                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * ILPvalidate:
 * ------------
 * arguments:
 * none.
 * description:
 * solves the game board using ILP with the Gurobi library.
 * prints error if fails.
 * if a solution was successfully found, stores the solution in the cells'
 * demiVal field.
 * returns 1 iff a solution was found.
 */
int ILPvalidate() {
	GRBenv   	*env   = NULL;
	GRBmodel 	*model = NULL;
	int      	error, result, N = myGame.N;
	double		*lb = calloc(N*N*N, sizeof(double));
	double		*solution = calloc(N*N*N, sizeof(double));
	char		*vtype = calloc(N*N*N, sizeof(char));
	int			*ind = calloc(N, sizeof(int));
	double		*val = calloc(N, sizeof(double));

	/*create environment*/
	error = GRBloadenv(&env, NULL);
	GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) goto END;

	/*create model*/
	prepareTypesAndBounds(vtype, lb);
	error = GRBnewmodel(env, &model, NULL , N*N*N, NULL, (double*)lb, NULL, (char*)vtype, NULL);
	if (error) goto END;

	/*add constraints imposed by cols, rows, blocks, and cells having to containt a single value*/
	error = addCellSingleValueConstraint(ind, val, model);
	if (error) goto END;
	error = addColsConstraints(ind, val, model);
	if (error) goto END;
	error = addRowConstraints(ind, val, model);
	if (error) goto END;
	error = addBlockConstraints(ind, val, model);
	if (error) goto END;
	error = GRBoptimize(model);
	if (error) goto END;

	/*check whether an optimal solution was found*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &result);
	if (error) goto END;
	if (result == 2)  {
		result = 1;
	} else {
		result = 0;
		goto END;
	}

	/*store the solution*/
	error = storeSolution(model, solution);
	if (error) goto END;

	/*free memory, report errors and return result*/
	END:
	if (error) printf("ERROR: %s\n", GRBgeterrormsg(env));
	GRBfreemodel(model);
	GRBfreeenv(env);
	free(lb);
	free(solution);
	free(vtype);
	free(ind);
	free(val);
	return result;
}



// wordle_solver.h
#ifndef WORDLE_SOLVER_H
#define WORDLE_SOLVER_H

#include "wordle.h"
#include <math.h>

void init_solver(const char* filename);
char* player_optimal(const char lastResult[WORD_LENGTH + 1]);

#endif
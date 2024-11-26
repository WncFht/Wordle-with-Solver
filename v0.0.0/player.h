#ifndef PLAYER_H
#define PLAYER_H

#include "wordle.h"

char* player_AI1(const char lastResult[WORD_LENGTH + 1]);
char* player_AI2(const char lastResult[WORD_LENGTH + 1]);

#endif
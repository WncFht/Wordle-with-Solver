#ifndef WORDLE_H
#define WORDLE_H

#define WORD_LENGTH 5
#define MAX_ROUNDS 10

typedef char* (*Player)(const char lastResult[WORD_LENGTH + 1]);

void wordle(const char solution[WORD_LENGTH + 1], Player *player);
// You can define more functions here
// For example:
// char* player_input(const char lastResult[WORD_LENGTH + 1]);
// char* player_random(const char lastResult[WORD_LENGTH + 1]);
#endif
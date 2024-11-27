#ifndef PLAYER_H
#define PLAYER_H

#include "wordle.h"

// ============= Player Strategy Functions =============

/**
 * Human player interface
 * @param lastResult Previous guess feedback
 * @return Player's input word
 */
char* player_input(const char lastResult[WORD_LENGTH + 1]);

/**
 * Random guessing strategy
 * @param lastResult Previous guess feedback
 * @return Randomly selected word
 */
char* player_random(const char lastResult[WORD_LENGTH + 1]);

/**
 * Information entropy based strategy
 * @param lastResult Previous guess feedback
 * @return Word with maximum information gain
 */
char* player_entropy(const char lastResult[WORD_LENGTH + 1]);

/**
 * Minimax strategy implementation
 * @param lastResult Previous guess feedback
 * @return Word that minimizes maximum possible solutions
 */
char* player_minimax(const char lastResult[WORD_LENGTH + 1]);

/**
 * Letter frequency based strategy
 * @param lastResult Previous guess feedback
 * @return Word based on letter frequency analysis
 */
char* player_frequency(const char lastResult[WORD_LENGTH + 1]);

/**
 * Decision tree based AI strategy
 * @param lastResult Previous guess feedback
 * @return Word chosen from decision tree
 */
char* player_AI(const char lastResult[WORD_LENGTH + 1]);

#endif
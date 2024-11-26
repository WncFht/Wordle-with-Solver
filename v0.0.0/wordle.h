/**
 * wordle.h - Core header file for Wordle game implementation
 *
 * This header defines the core game constants, types, and function interfaces
 * for the Wordle word-guessing game. It provides the foundation for both
 * game logic and player strategies.
 */

#ifndef WORDLE_H
#define WORDLE_H

#include <stdbool.h>

// ============= Game Constants =============
#define WORD_LENGTH 5        // Length of each word in the game
#define MAX_ROUNDS 10        // Maximum number of guessing rounds allowed
#define MAX_WORDS 15000      // Maximum number of words in the dictionary
#define SOLUTION_LENGTH 2500 // Maximum number of possible solution words

/**
 * Player strategy function type
 * @param lastResult Previous guess feedback ('G'=correct position, 
 *                  'Y'=wrong position, 'B'=not in word)
 * @return Next word to guess, must be allocated dynamically
 */
typedef char* (*Player)(const char lastResult[WORD_LENGTH + 1]);

// ============= Core Game Functions =============

/**
 * Loads the dictionary of valid words from file
 * @param filename Path to dictionary file
 */
void loadWords(const char* filename);

/**
 * Loads the list of possible solution words
 * @param filename Path to solutions file
 */
void loadSolution(const char* filename);

/**
 * Evaluates a guess against the solution
 * @param solution Target word
 * @param guess Player's guessed word
 * @return Feedback string (G/Y/B pattern)
 */
char* checkWord(const char solution[WORD_LENGTH + 1], 
                const char guess[WORD_LENGTH + 1]);

/**
 * Main game control function
 * @param solution Word to be guessed
 * @param player Pointer to player strategy function
 */
void wordle(const char solution[WORD_LENGTH + 1], Player *player);

/**
 * Gets a random word from the dictionary
 * @return Random word string
 */
char* get_random_word(void);

// ============= Utility Functions =============

/**
 * Checks if a guess is legal (exists in dictionary)
 * @param guess Word to check
 * @return false if word is legal, true if illegal
 */
bool not_legal(const char* guess);

/**
 * Validates if a word is in solution list
 * @param solution Word to check
 * @return false if valid, true if invalid
 */
bool invalid(const char* solution);

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
char* player_mimmax(const char lastResult[WORD_LENGTH + 1]);

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

// ============= Global Data =============
extern char wordList[MAX_WORDS][WORD_LENGTH + 1];        // Dictionary words
extern int wordCount;                                    // Number of words loaded
extern char solutionList[SOLUTION_LENGTH][WORD_LENGTH + 1]; // Possible solutions
extern int solutionCount;                               // Number of solutions

#endif
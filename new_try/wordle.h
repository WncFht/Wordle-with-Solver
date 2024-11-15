// wordle.h
#ifndef WORDLE_H
#define WORDLE_H

#define WORD_LENGTH 5
#define MAX_ROUNDS 10

// Player function type - takes last result, returns next guess
typedef char* (*Player)(const char lastResult[WORD_LENGTH + 1]);

// Core game function
void wordle(const char solution[WORD_LENGTH + 1], Player player);

// Word validation
int is_valid_word(const char* word);

// Load word list from file
int load_word_list(const char* filename);

// Get random word from loaded list
char* get_random_word(void);

// Helper function to generate feedback
void generate_feedback(const char* solution, const char* guess, char* feedback);

// Example player implementations
char* player_input(const char lastResult[WORD_LENGTH + 1]);  // Human player
char* player_random(const char lastResult[WORD_LENGTH + 1]); // Random guessing

#endif // WORDLE_H
// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wordle.h"

void print_instructions() {
    printf("\n=== Welcome to Wordle ===\n");
    printf("Try to guess the 5-letter word. You have %d attempts.\n", MAX_ROUNDS);
    printf("After each guess, you'll get feedback:\n");
    printf("G = Correct letter in correct position\n");
    printf("Y = Correct letter in wrong position\n");
    printf("B = Letter not in the word\n");
    printf("All input should be in UPPERCASE letters.\n\n");
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    
    // Load word list with error reporting
    int loaded_words = load_word_list("wordList.txt");
    if (loaded_words == 0) {
        printf("Error: Could not load word list\n");
        return 1;
    }
    printf("Loaded %d words\n", loaded_words);  // Debug info
    
    char* solution = get_random_word();
    if (!solution) {
        printf("Error: Could not get random word\n");
        return 1;
    }
    
    print_instructions();
    
    // Start game with human player
    wordle(solution, player_entropy);
    
    return 0;
}
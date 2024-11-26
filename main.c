// main1.c
/**
 * main1.c - Main program entry point for Wordle game
 *
 * Handles program initialization and game start.
 * Uses AI player strategy (player_AI2)
 */

#include "wordle.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int main() {
    // Read solution word
    char solution[WORD_LENGTH + 1];
    scanf("%s", solution);
    
    // Convert to uppercase for consistency
    for(int i = 0; solution[i]; i++) {
        solution[i] = toupper(solution[i]);
    }
    
    // Set player strategy and start game
    Player player = player_frequency;
    wordle(solution, &player);
    return 0;
}
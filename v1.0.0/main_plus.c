/**
 * main.c - Test harness for Wordle game strategies
 *
 * This program implements an automated testing system for Wordle game strategies.
 * It runs multiple test cases with random solution words and collects statistics
 * on strategy performance including success rate, average score, and guesses.
 */

#include "wordle.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#define TEST_CASES 1000  // Number of test cases to run

/**
 * Structure to store results of each test case
 */
typedef struct {
    char word[WORD_LENGTH + 1];  // Solution word
    int score;                   // Score achieved (0-10)
    int guesses;                 // Number of guesses used
} TestResult;

// Global variables to capture test results
static char last_output[256] = {0};  // Captures wordle output
static int found_score = 0;          // Score from last test
static int found_guesses = 0;        // Guesses from last test

/**
 * Simplified version of wordle function for testing
 * Tracks score and number of guesses used
 *
 * @param solution Target word to guess
 * @param player Pointer to player strategy function
 */
void test_wordle(const char solution[WORD_LENGTH + 1], Player *player) {
    // Initialize dictionaries if needed
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    if (invalid(solution)) {
        return;
    }
    
    // Initialize feedback for first guess
    char* lastResult = (char*)malloc(WORD_LENGTH * sizeof(char));
    for (int i = 0; i < WORD_LENGTH; i++) {
        lastResult[i] = ' ';
    }
    lastResult[WORD_LENGTH] = '\0';
    
    // Main game loop
    int i;
    for (i = 0; i < MAX_ROUNDS; i++) {
        char* guess = (*player)(lastResult);
        if (not_legal(guess)) {
            free(guess);
            continue;
        }
        
        char* result = checkWord(solution, guess);
        if (strcmp(solution, guess) == 0) {
            // Success - calculate score and guesses
            found_score = 10 - i;
            found_guesses = i + 1;
            free(guess);
            free(result);
            free(lastResult);
            return;
        }
        
        free(guess);
        free(lastResult);
        lastResult = result;
    }
    
    // Failure case
    found_score = 0;
    found_guesses = MAX_ROUNDS;
    free(lastResult);
}

/**
 * Gets a random unused solution word
 * 
 * @param used Array tracking used words
 * @param used_count Pointer to count of used words
 * @return Random unused solution word or NULL if none left
 */
char* get_solution(bool* used, int* used_count) {
    if (*used_count >= SOLUTION_LENGTH) {
        return NULL;
    }
    
    // Find unused random word
    int index;
    do {
        index = rand() % SOLUTION_LENGTH;
    } while (used[index]);
    
    used[index] = true;
    (*used_count)++;
    
    return solutionList[index];
}

/**
 * Main program entry point
 * Runs automated tests of player strategy
 */
int main() {
    srand(time(NULL));  // Initialize random seed
    
    // Load dictionaries if needed
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    
    // Initialize test tracking
    bool* used = (bool*)calloc(SOLUTION_LENGTH, sizeof(bool));
    int used_count = 0;
    
    // Initialize results storage
    TestResult* results = (TestResult*)malloc(TEST_CASES * sizeof(TestResult));
    int total_score = 0;
    int total_guesses = 0;
    int successful_tests = 0;
    
    // Select player strategy
    Player player = player_AI;
    printf("\nStarting %d test cases...\n", TEST_CASES);
    
    // Run test cases
    for (int i = 0; i < TEST_CASES && i < SOLUTION_LENGTH; i++) {
        // Get next test word
        char* solution = get_solution(used, &used_count);
        if (!solution) {
            printf("Run out of solutions after %d tests\n", i);
            break;
        }
        
        // Store solution and reset result trackers
        strcpy(results[i].word, solution);
        found_score = 0;
        found_guesses = 0;
        
        // Run test and store results
        test_wordle(solution, &player);
        results[i].score = found_score;
        results[i].guesses = found_guesses;
        
        // Update statistics
        if (found_score > 0) {
            total_score += found_score;
            total_guesses += found_guesses;
            successful_tests++;
            printf("Test %d/%d: %s - Score: %d, Guesses: %d\n", 
                   i + 1, TEST_CASES, solution, found_score, found_guesses);
        } else {
            printf("Test %d/%d: %s - Failed\n", i + 1, TEST_CASES, solution);
        }
        
        printf("\n");
        
        // Print intermediate results every 100 tests
        if ((i + 1) % 100 == 0) {
            printf("\nIntermediate results after %d tests:\n", i + 1);
            printf("Successful tests so far: %d\n", successful_tests);
            printf("Current success rate: %.2f%%\n", 
                   (float)successful_tests / (i + 1) * 100);
            printf("Current average score: %.2f\n", 
                   (float)total_score / successful_tests);
            printf("Current average guesses: %.2f\n\n", 
                   (float)total_guesses / successful_tests);
        }
    }
    
    // Print final statistics
    printf("\nFinal Test Summary:\n");
    printf("Total tests: %d\n", TEST_CASES);
    printf("Successful tests: %d\n", successful_tests);
    printf("Success rate: %.2f%%\n", 
           (float)successful_tests / TEST_CASES * 100);
    printf("Average score: %.2f\n", 
           (float)total_score / successful_tests);
    printf("Average guesses: %.2f\n", 
           (float)total_guesses / successful_tests);
    
    // Cleanup
    free(used);
    free(results);
    
    return 0;
}
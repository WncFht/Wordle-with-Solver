/**
 * player.c - Player strategy implementations for Wordle game
 *
 * This file implements various player strategies including:
 * 1. Human input (player_input)
 * 2. Random guessing (player_random)
 * 3. Letter frequency based (player_frequency)
 * 4. Minimax strategy (player_minax)
 * 5. Information entropy based (player_entropy)
 * 6. Decision tree based (player_AI)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "wordle.h"

// ============= Constants =============
#define MAX_WORDS 15000
#define PATTERN_COUNT 243  // Number of possible feedback patterns (3^5)
#define MAX_LINE_LENGTH 256
#define MAX_LINES 10000

// ============= Function Declarations =============
static void to_uppercase(char* str);
static void init_possible_solutions(void);
static void update_solutions(const char* guess, const char* feedback);
static float calculate_entropy(int* pattern_counts);
static void generate_pattern_counts(const char* word, int* pattern_counts);
void cleanup_ai(void);
static void load_decision_tree(void);
static const char* find_next_move(const char* feedback, int level);

// ============= Global Variables =============
static char input_buffer[WORD_LENGTH + 1];
static char** possible_solutions = NULL;
static int solution_count = 0;
static char last_guess[WORD_LENGTH + 1] = {0};
static char best_guess_buffer[WORD_LENGTH + 1] = {0};
extern char wordList[MAX_WORDS][WORD_LENGTH + 1];
extern int wordCount;

// Decision tree related globals
static char** decision_lines = NULL;
static int line_count = 0;
static char current_word[6] = "SALET";
static bool first_guess = true;
static char cumulative_pattern[MAX_LINE_LENGTH] = "";

// ============= Utility Functions =============

/**
 * Checks if a word exists in the dictionary
 * @param word The word to check
 * @return 1 if word is valid, 0 otherwise
 */
int is_valid_word(const char* word) {
    if (strlen(word) != WORD_LENGTH) return 0;
    
    char upper_word[WORD_LENGTH + 1];
    strcpy(upper_word, word);
    for (int i = 0; upper_word[i]; i++) {
        upper_word[i] = toupper(upper_word[i]);
    }
    
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(wordList[i], upper_word) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Converts a string to uppercase
 * @param str String to convert
 */
static void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

/**
 * Initializes the set of possible solutions
 * Copies all words from the word list to the solution set
 */
static void init_possible_solutions(void) {
    if (possible_solutions != NULL) {
        cleanup_ai();
    }
    
    possible_solutions = (char**)malloc(MAX_WORDS * sizeof(char*));
    if (!possible_solutions) return;
    
    for (int i = 0; i < MAX_WORDS; i++) {
        possible_solutions[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!possible_solutions[i]) {
            for (int j = 0; j < i; j++) {
                free(possible_solutions[j]);
            }
            free(possible_solutions);
            possible_solutions = NULL;
            return;
        }
    }
    
    for (int i = 0; i < wordCount; i++) {
        strcpy(possible_solutions[i], wordList[i]);
    }
    solution_count = wordCount;
}

/**
 * Updates the set of possible solutions based on guess feedback
 * @param guess The guessed word
 * @param feedback The feedback pattern received
 */
static void update_solutions(const char* guess, const char* feedback) {
    if (!possible_solutions || solution_count == 0) return;
    
    char** temp = (char**)malloc(solution_count * sizeof(char*));
    if (!temp) return;
    
    for (int i = 0; i < solution_count; i++) {
        temp[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!temp[i]) {
            for (int j = 0; j < i; j++) free(temp[j]);
            free(temp);
            return;
        }
    }
    
    int new_count = 0;
    for (int i = 0; i < solution_count; i++) {
        char* test_feedback = checkWord(possible_solutions[i], guess);
        if (!test_feedback) {
            for (int j = 0; j < solution_count; j++) free(temp[j]);
            free(temp);
            return;
        }
        if (strcmp(test_feedback, feedback) == 0) {
            strcpy(temp[new_count++], possible_solutions[i]);
        }
        free(test_feedback);
    }
    
    for (int i = 0; i < new_count; i++) {
        strcpy(possible_solutions[i], temp[i]);
    }
    solution_count = new_count;
    
    for (int i = 0; i < solution_count; i++) {
        free(temp[i]);
    }
    free(temp);
}

/**
 * Cleans up AI-related memory allocations
 */
void cleanup_ai(void) {
    if (possible_solutions) {
        for (int i = 0; i < MAX_WORDS; i++) {
            free(possible_solutions[i]);
        }
        free(possible_solutions);
        possible_solutions = NULL;
    }
    solution_count = 0;
}

/**
 * Calculates information entropy for a given pattern distribution
 * @param pattern_counts Array of pattern counts
 * @return Entropy value
 */
static float calculate_entropy(int* pattern_counts) {
    float entropy = 0.0f;
    for (int i = 0; i < PATTERN_COUNT; i++) {
        if (pattern_counts[i] > 0) {
            float p = (float)pattern_counts[i] / solution_count;
            entropy -= p * log2f(p);
        }
    }
    return entropy;
}

/**
 * Generates pattern counts for a given word
 * @param word Word to evaluate
 * @param pattern_counts Output array for pattern counts
 */
static void generate_pattern_counts(const char* word, int* pattern_counts) {
    memset(pattern_counts, 0, PATTERN_COUNT * sizeof(int));
    
    for (int i = 0; i < solution_count; i++) {
        char* test_feedback = checkWord(possible_solutions[i], word);
        if (!test_feedback) continue;
        
        int pattern_index = 0;
        for (int j = 0; j < WORD_LENGTH; j++) {
            pattern_index = pattern_index * 3 + 
                (test_feedback[j] == 'G' ? 2 : 
                 test_feedback[j] == 'Y' ? 1 : 0);
        }
        pattern_counts[pattern_index]++;
        free(test_feedback);
    }
}

// ============= Basic Player Implementations =============

/**
 * Human player interface - handles user input
 * @param lastResult Feedback from previous guess
 * @return Player's input word
 */
char* player_input(const char lastResult[WORD_LENGTH + 1]) {
    static char input_buffer[WORD_LENGTH * 2];
    
    if (lastResult && *lastResult) {
        printf("Feedback: %s\n", lastResult);
    }
    
    while (1) {
        printf("Enter your guess: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            continue;
        }
        
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        if (strlen(input_buffer) == 0) continue;
        
        to_uppercase(input_buffer);
        
        if (strlen(input_buffer) != WORD_LENGTH) {
            printf("Please enter a %d-letter word.\n", WORD_LENGTH);
            continue;
        }
        
        int valid = 1;
        for (int i = 0; input_buffer[i]; i++) {
            if (!isalpha(input_buffer[i])) {
                printf("Please enter only letters.\n");
                valid = 0;
                break;
            }
        }
        if (!valid) continue;
        
        if (!is_valid_word(input_buffer)) {
            printf("Word not in dictionary. Try again.\n");
            continue;
        }
        
        return input_buffer;
    }
}

/**
 * Random guessing strategy
 * @param lastResult Feedback from previous guess
 * @return Randomly chosen word from dictionary
 */
char* player_random(const char lastResult[WORD_LENGTH + 1]) {
    static char random_guess[WORD_LENGTH + 1];
    char* word = get_random_word();
    
    if (!word) return NULL;
    
    strncpy(random_guess, word, WORD_LENGTH);
    random_guess[WORD_LENGTH] = '\0';
    return random_guess;
}

/**
 * Letter frequency based guessing strategy
 * @param lastResult Feedback from previous guess
 * @return Best guess based on letter frequencies
 */
char* player_frequency(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    static char best_guess[WORD_LENGTH + 1];
    
    // Initialize on first guess
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");  // Start with STARE as first guess
        strcpy(last_guess, guess);
        return guess;
    }
    
    // Process previous guess result
    if (lastResult && *lastResult && strlen(last_guess) > 0) {
        update_solutions(last_guess, lastResult);
        printf("Remaining possible solutions: %d\n", solution_count);
        
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        } else if (solution_count == 0) {
            cleanup_ai();
            init_possible_solutions();
            strcpy(guess, "STARE");
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    // Calculate letter frequencies
    float letter_freq[26][WORD_LENGTH] = {0};
    float total_freq[26] = {0};
    
    for (int i = 0; i < solution_count; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            int letter = possible_solutions[i][j] - 'A';
            letter_freq[letter][j]++;
            total_freq[letter]++;
        }
    }
    
    // Normalize frequencies
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            letter_freq[i][j] /= solution_count;
        }
        total_freq[i] /= (solution_count * WORD_LENGTH);
    }
    
    // Find best word based on letter frequencies
    float best_score = -1.0f;
    
    for (int i = 0; i < wordCount; i++) {
        float score = 0.0f;
        int used[26] = {0};
        
        for (int j = 0; j < WORD_LENGTH; j++) {
            int letter = wordList[i][j] - 'A';
            score += letter_freq[letter][j] * 2.0f;  // Position-specific score
            
            if (!used[letter]) {
                score += total_freq[letter];  // Overall letter frequency score
                used[letter] = 1;
            }
        }
        
        if (score > best_score) {
            best_score = score;
            strcpy(best_guess, wordList[i]);
        }
    }
    
    if (best_score > -1.0f) {
        strcpy(guess, best_guess);
    } else {
        strcpy(guess, possible_solutions[0]);
    }
    
    strcpy(last_guess, guess);
    printf("Selected guess: %s (score: %.2f)\n", guess, best_score);
    return guess;
}

/**
 * Minimax strategy implementation
 * Minimizes the maximum possible remaining solutions
 * @param lastResult Feedback from previous guess
 * @return Best guess based on minimax strategy
 */
char* player_minimax(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    static char best_guess[WORD_LENGTH + 1];
    
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");
        strcpy(last_guess, guess);
        return guess;
    }
    
    if (lastResult && *lastResult && strlen(last_guess) > 0) {
        update_solutions(last_guess, lastResult);
        
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    int min_worst_case = MAX_WORDS;
    int* pattern_counts = (int*)calloc(PATTERN_COUNT, sizeof(int));
    
    if (!pattern_counts) {
        strcpy(guess, possible_solutions[0]);
        strcpy(last_guess, guess);
        return guess;
    }
    
    for (int i = 0; i < wordCount; i++) {
        generate_pattern_counts(wordList[i], pattern_counts);
        
        int max_remaining = 0;
        for (int j = 0; j < PATTERN_COUNT; j++) {
            if (pattern_counts[j] > max_remaining) {
                max_remaining = pattern_counts[j];
            }
        }
        
        if (max_remaining < min_worst_case) {
            min_worst_case = max_remaining;
            strcpy(best_guess, wordList[i]);
        }
    }
    
    free(pattern_counts);
    strcpy(guess, best_guess);
    strcpy(last_guess, guess);
    return guess;
}

/**
 * Information entropy based strategy
 * Chooses words that maximize information gain
 * @param lastResult Feedback from previous guess
 * @return Best guess based on information entropy
 */
char* player_entropy(const char lastResult[WORD_LENGTH + 1]) {
    char* guess = (char*)malloc(WORD_LENGTH + 1);
    if (!guess) return NULL;
    
    printf("Last result: %s\n", lastResult);
    
    // Initialize on first guess
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");  // Use STARE as fixed starting word
        strcpy(last_guess, guess);
        return guess;
    }
    
    // Process previous guess result
    if (lastResult && *lastResult) {
        update_solutions(last_guess, lastResult);
        printf("Remaining possible solutions: %d\n", solution_count);
        
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        } else if (solution_count == 0) {
            cleanup_ai();
            init_possible_solutions();
            strcpy(guess, "STARE");
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    // Allocate pattern counts array
    int* pattern_counts = (int*)calloc(PATTERN_COUNT, sizeof(int));
    if (!pattern_counts) {
        strcpy(guess, possible_solutions[0]);
        strcpy(last_guess, guess);
        return guess;
    }
    
    float max_entropy = -1.0f;
    const char* best_word = NULL;
    
    if (solution_count <= 2) {
        // When few solutions remain, choose directly from them
        strcpy(guess, possible_solutions[0]);
    } else {
        // Evaluate all words to find optimal guess
        for (int i = 0; i < wordCount; i++) {
            generate_pattern_counts(wordList[i], pattern_counts);
            float entropy = calculate_entropy(pattern_counts);
            
            float solution_entropy_sum = 0.0f; // 解集中单词的总熵
            int in_solution = 0;              // 是否属于解集的标志

            // 遍历单词表计算解集中熵的总和
            for (int j = 0; j < solution_count; j++) {
                if (strcmp(wordList[i], possible_solutions[j]) == 0) {
                    in_solution = 1;
                    solution_entropy_sum += entropy;
                    break;
                }
            }

            // 根据是否在解集中调整熵
            if (in_solution) {
                float solution_factor = entropy / solution_entropy_sum; // 单词在解集中熵的占比
                entropy *= (1.0f + solution_factor); // 放大熵值
            }

            
            if (entropy > max_entropy) {
                max_entropy = entropy;
                best_word = wordList[i];
            }
        }
        
        if (best_word) {
            strcpy(guess, best_word);
        } else {
            strcpy(guess, possible_solutions[0]);
        }
    }
    
    free(pattern_counts);
    
    strcpy(last_guess, guess);
    printf("Selected guess: %s (entropy: %.2f)\n", guess, max_entropy);
    return guess;
}

/**
 * Decision tree based strategy
 * Uses pre-computed decision tree to make optimal moves
 * @param lastResult Feedback from previous guess
 * @return Best guess based on decision tree
 */
char* player_AI(const char lastResult[WORD_LENGTH + 1]) {
    load_decision_tree();
    char* guess = malloc(WORD_LENGTH + 1);
    if (!guess) return NULL;

    // Check if this is the first guess
    bool is_first_guess = true;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (lastResult[i] != ' ') {
            is_first_guess = false;
            break;
        }
    }

    // Start with SALET on first guess
    if (is_first_guess) {
        strcpy(guess, "SALET");
        strcpy(current_word, "SALET");
        cumulative_pattern[0] = '\0';  // Clear cumulative pattern
        return guess;
    }

    printf("Current word: %s\n", current_word);

    // Calculate level based on cumulative pattern length/state
    int level = 1;
    for (char* p = cumulative_pattern; *p; p++) {
        if (*p >= '1' && *p <= '9') level++;
    }

    // Find next move from decision tree
    const char* next = find_next_move(lastResult, level);
    if (next) {
        strcpy(guess, next);
        strcpy(current_word, next);
        free((void*)next);
        return guess;
    }

    free(guess);
    return NULL;
}

/**
 * Loads the decision tree from file
 * Called internally by player_AI
 */
static void load_decision_tree(void) {
    static bool loaded = false;
    if (loaded) return;

    FILE* file = fopen("tree_u.txt", "r");
    if (!file) {
        printf("Failed to open tree.txt\n");
        return;
    }

    // Allocate memory for decision lines
    decision_lines = (char**)malloc(MAX_LINES * sizeof(char*));
    if (!decision_lines) {
        fclose(file);
        return;
    }

    // Read and store each line
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && line_count < MAX_LINES) {
        line[strcspn(line, "\n")] = 0;
        
        decision_lines[line_count] = strdup(line);
        if (!decision_lines[line_count]) {
            for (int i = 0; i < line_count; i++) {
                free(decision_lines[i]);
            }
            free(decision_lines);
            decision_lines = NULL;
            fclose(file);
            return;
        }
        line_count++;
    }

    fclose(file);
    loaded = true;
    printf("Loaded %d decision lines\n", line_count);
}

/**
 * Finds the next move from the decision tree
 * @param feedback Current feedback pattern
 * @param level Current depth in the decision tree
 * @return Next word to guess, or NULL if not found
 */
static const char* find_next_move(const char* feedback, int level) {
    // Append new pattern to cumulative pattern
    char new_pattern[32];
    sprintf(new_pattern, "%s %s%d ", current_word, feedback, level);
    strcat(cumulative_pattern, new_pattern);
    
    printf("Looking for pattern: '%s'\n", cumulative_pattern);
    
    // Search for matching line
    for (int i = 0; i < line_count; i++) {
        if (strstr(decision_lines[i], cumulative_pattern) == decision_lines[i]) {
            // Extract next word
            const char* line = decision_lines[i] + strlen(cumulative_pattern);
            char next_word[WORD_LENGTH + 1];
            if (sscanf(line, "%5s", next_word) == 1) {
                printf("Found next word: %s in line: %s\n", next_word, decision_lines[i]);
                return strdup(next_word);
            }
        }
    }
    
    printf("No matching move found\n");
    return NULL;
}
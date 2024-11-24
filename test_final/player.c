// players.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "wordle.h"


extern char wordList[MAX_WORDS][WORD_LENGTH + 1];
extern int wordCount;

#define MAX_LINE_LENGTH 256
#define MAX_LINES 10000

static char** decision_lines = NULL;
static int line_count = 0;
static char current_word[6] = "salet";
static char cumulative_pattern[MAX_LINE_LENGTH] = "";  // 累积的pattern串

static void load_decision_tree(void) {
    static bool loaded = false;
    if (loaded) return;

    FILE* file = fopen("tree_l.txt", "r");
    if (!file) {
        printf("Failed to open tree_l.txt\n");
        return;
    }

    decision_lines = (char**)malloc(MAX_LINES * sizeof(char*));
    if (!decision_lines) {
        fclose(file);
        return;
    }

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
        
        // printf("Loaded line %d: %s\n", line_count, decision_lines[line_count]);
        line_count++;
    }

    fclose(file);
    loaded = true;
    // printf("Loaded %d decision lines\n", line_count);
}

static const char* find_next_move(const char* feedback, int level) {
    // 在累积pattern末尾追加新pattern
    char new_pattern[32];
    sprintf(new_pattern, "%s %s%d ", current_word, feedback, level);
    strcat(cumulative_pattern, new_pattern);
    
    // printf("Looking for pattern: '%s'\n", cumulative_pattern);
    
    // 查找匹配的行
    for (int i = 0; i < line_count; i++) {
        // printf("Comparing with line %d: %s\n", i, decision_lines[i]);
        if (strstr(decision_lines[i], cumulative_pattern) == decision_lines[i]) {
            // 提取下一个单词
            const char* line = decision_lines[i] + strlen(cumulative_pattern);
            char next_word[WORD_LENGTH + 1];
            if (sscanf(line, "%5s", next_word) == 1) {
                // printf("Found next word: %s in line: %s\n", next_word, decision_lines[i]);
                return strdup(next_word);
            }
        }
    }
    
    // printf("No matching move found\n");
    return NULL;
}

char* player_AI(const char lastResult[WORD_LENGTH + 1]) {
    load_decision_tree();
    // printf("Last result: %s\n", lastResult);
    char* guess = malloc(WORD_LENGTH + 1);
    if (!guess) return NULL;

    bool is_first_guess = true;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (lastResult[i] != ' ') {
            is_first_guess = false;
            break;
        }
    }

    if (is_first_guess) {
        // printf("First guess: SALET\n");
        strcpy(guess, "salet");
        strcpy(current_word, "salet");
        cumulative_pattern[0] = '\0';  // 清空累积pattern
        return guess;
    }

    // printf("Current word: %s\n", current_word);
    // printf("Last result: %s\n", lastResult);

    // 通过累积pattern的长度/状态计算level
    int level = 1;
    for (char* p = cumulative_pattern; *p; p++) {
        if (*p >= '1' && *p <= '9') level++;
    }

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
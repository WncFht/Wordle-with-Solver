// wordle.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "wordle.h"

WordleStatus wordle_init(WordleGame* game, const char* wordListPath) {
    // 分配内存
    game->wordList = (char**)malloc(MAX_WORDS * sizeof(char*));
    if (!game->wordList) return WORDLE_MEMORY_ERROR;
    
    game->solutions = (char**)malloc(SOLUTION_WORDS * sizeof(char*));
    if (!game->solutions) {
        free(game->wordList);
        return WORDLE_MEMORY_ERROR;
    }
    
    // 分配单词空间
    for (int i = 0; i < MAX_WORDS; i++) {
        game->wordList[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!game->wordList[i]) {
            for (int j = 0; j < i; j++) free(game->wordList[j]);
            free(game->wordList);
            free(game->solutions);
            return WORDLE_MEMORY_ERROR;
        }
    }
    
    for (int i = 0; i < SOLUTION_WORDS; i++) {
        game->solutions[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!game->solutions[i]) {
            for (int j = 0; j < i; j++) free(game->solutions[j]);
            for (int j = 0; j < MAX_WORDS; j++) free(game->wordList[j]);
            free(game->wordList);
            free(game->solutions);
            return WORDLE_MEMORY_ERROR;
        }
    }
    
    // 读取单词列表
    FILE* file = fopen(wordListPath, "r");
    if (!file) return WORDLE_FILE_ERROR;
    
    game->wordCount = 0;
    game->solutionCount = 0;
    char word[256];
    
    while (fgets(word, sizeof(word), file) && game->wordCount < MAX_WORDS) {
        word[strcspn(word, "\n")] = 0;
        if (strlen(word) == WORD_LENGTH) {
            for (int i = 0; word[i]; i++) {
                word[i] = toupper(word[i]);
            }
            strcpy(game->wordList[game->wordCount], word);
            if (game->solutionCount < SOLUTION_WORDS) {
                strcpy(game->solutions[game->solutionCount], word);
                game->solutionCount++;
            }
            game->wordCount++;
        }
    }
    
    fclose(file);
    game->remainingGuesses = MAX_GUESSES;
    return WORDLE_SUCCESS;
}

WordleStatus wordle_set_solution(WordleGame* game, const char* solution) {
    if (solution) {
        if (strlen(solution) != WORD_LENGTH) return WORDLE_INVALID_INPUT;
        strcpy(game->currentSolution, solution);
    } else {
        int idx = rand() % game->solutionCount;
        strcpy(game->currentSolution, game->solutions[idx]);
    }
    game->remainingGuesses = MAX_GUESSES;
    return WORDLE_SUCCESS;
}

WordleStatus wordle_make_guess(WordleGame* game, const char* guess, char* feedback) {
    if (!guess || strlen(guess) != WORD_LENGTH) return WORDLE_INVALID_INPUT;
    if (game->remainingGuesses <= 0) return WORDLE_INVALID_INPUT;
    
    char upperGuess[WORD_LENGTH + 1];
    strcpy(upperGuess, guess);
    for (int i = 0; upperGuess[i]; i++) {
        upperGuess[i] = toupper(upperGuess[i]);
    }
    
    if (!wordle_is_valid_word(game, upperGuess)) {
        return WORDLE_INVALID_WORD;
    }
    
    int used[WORD_LENGTH] = {0};
    
    // 标记完全匹配的字母
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (upperGuess[i] == game->currentSolution[i]) {
            feedback[i] = 'G';
            used[i] = 1;
        } else {
            feedback[i] = 'B';
        }
    }
    
    // 标记位置不正确的字母
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == 'B') {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (!used[j] && upperGuess[i] == game->currentSolution[j]) {
                    feedback[i] = 'Y';
                    used[j] = 1;
                    break;
                }
            }
        }
    }
    
    feedback[WORD_LENGTH] = '\0';
    game->remainingGuesses--;
    
    return WORDLE_SUCCESS;
}

int wordle_is_valid_word(WordleGame* game, const char* word) {
    for (int i = 0; i < game->wordCount; i++) {
        if (strcmp(game->wordList[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

void wordle_cleanup(WordleGame* game) {
    if (game->wordList) {
        for (int i = 0; i < MAX_WORDS; i++) {
            free(game->wordList[i]);
        }
        free(game->wordList);
    }
    
    if (game->solutions) {
        for (int i = 0; i < SOLUTION_WORDS; i++) {
            free(game->solutions[i]);
        }
        free(game->solutions);
    }
}

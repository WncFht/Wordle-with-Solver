#include "wordle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

char wordList[MAX_WORDS][WORD_LENGTH + 1];
int wordCount = 0;
char solutionList[SOLUTION_LENGTH][WORD_LENGTH + 1];
int solutionCount = 0;

char* checkWord(const char solution[WORD_LENGTH + 1], const char guess[WORD_LENGTH + 1]) {
    char* match_g = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
    char matched_solution[WORD_LENGTH] = {0};
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == solution[i]) {
            match_g[i] = 'G';
            matched_solution[i] = 'G';
        } else {
            match_g[i] = 'B';
        }
    }
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (match_g[i] == 'B') {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (matched_solution[j] == 0 && guess[i] == solution[j]) {
                    match_g[i] = 'Y';
                    matched_solution[j] = 'Y';
                    break;
                }
            }
        }
    }
    match_g[WORD_LENGTH] = '\0';
    return match_g;
}

void loadWords(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    char buffer[WORD_LENGTH + 2];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        // 转换为大写
        for(int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        strcpy(wordList[wordCount], buffer);
        wordList[wordCount][WORD_LENGTH] = '\0';
        wordCount++;
        if (wordCount >= MAX_WORDS) break;
    }
    printf("Loaded %d words from %s\n", wordCount, filename);
    fclose(file);
}

void loadSolution(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    char buffer[WORD_LENGTH + 2];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        // 转换为大写
        for(int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        strcpy(solutionList[solutionCount], buffer);
        solutionList[solutionCount][WORD_LENGTH] = '\0';
        solutionCount++;
        if (solutionCount >= SOLUTION_LENGTH) break;
    }
    printf("Loaded %d solutions from %s\n", solutionCount, filename);
    fclose(file);
}

void wordle(const char solution[WORD_LENGTH + 1], Player *player) {
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    if (invalid(solution)) {
        return;
    }
    
    printf("Starting game with solution: %s\n", solution);
    
    char* lastResult = (char*)malloc(WORD_LENGTH * sizeof(char));
    for (int i = 0; i < WORD_LENGTH; i++) {
        lastResult[i] = ' ';
    }
    lastResult[WORD_LENGTH] = '\0';
    
    int i;
    for (i = 0; i < MAX_ROUNDS; i++) {
        char* guess = (*player)(lastResult);
        printf("Round %d: Guessing %s\n", i+1, guess);
        
        while (not_legal(guess)) {
            printf("Illegal guess, trying again\n");
            guess = (*player)(lastResult);
        }
        
        char* result = checkWord(solution, guess);
        printf("Feedback: %s\n", result);
        
        if (strcmp(solution, guess) == 0) {
            printf("Correct guess!\n");
            break;
        }
        
        free(guess);
        free(lastResult);
        lastResult = result;
    }
    
    if (i == MAX_ROUNDS) {
        printf("Failed to guess the word\n");
        printf("0\n");
    } else {
        printf("Succeeded in %d guesses\n", i+1);
        printf("%d\n", 10 - i);
    }
    
    free(lastResult);
}

bool not_legal(const char* guess) {
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(wordList[i], guess) == 0) {
            return false;
        }
    }
    return true;
}

// bool invalid(const char* solution) {
//     for (int i = 0; i < SOLUTION_LENGTH; i++) {
//         if (strcmp(solution, solutionList[i]) == 0) {
//             return false;
//         }
//     }
//     return true;
// }

bool invalid(const char* solution) {
    printf("Checking solution: %s\n", solution);
    for (int i = 0; i < SOLUTION_LENGTH; i++) {
        if (strcmp(solution, solutionList[i]) == 0) {
            printf("Solution is valid\n");
            return false;
        }
    }
    printf("Solution is not in the solution list\n");
    return true;
}
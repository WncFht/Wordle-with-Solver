// wordle.c
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "wordle.h"


// 增加词表大小
char wordList[MAX_WORDS][WORD_LENGTH + 1];
int wordCount = 0;

int load_wordList(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;  
    
    wordCount = 0;
    char word[100];
    
    while (fgets(word, sizeof(word), fp) && wordCount < MAX_WORDS) {
        // Remove newline and convert to uppercase
        word[strcspn(word, "\n")] = 0;
        if (strlen(word) == WORD_LENGTH) {
            for (int i = 0; word[i]; i++) {
                wordList[wordCount][i] = toupper(word[i]);
            }
            wordList[wordCount][WORD_LENGTH] = '\0';
            wordCount++;
        }
    }
    
    fclose(fp);
    return wordCount;
}

char* checkWord(const char solution[WORD_LENGTH + 1], const char guess[WORD_LENGTH + 1]) {
    char* result = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
    if (!result) return NULL;
    
    int used[WORD_LENGTH] = {0};
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == solution[i]) {
            result[i] = 'G';
            used[i] = 1;
        } else {
            result[i] = 'B';
        }
    }
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (result[i] == 'B') {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (!used[j] && guess[i] == solution[j]) {
                    result[i] = 'Y';
                    used[j] = 1;
                    break;
                }
            }
        }
    }
    
    result[WORD_LENGTH] = '\0';
    return result;
}

void wordle(const char solution[WORD_LENGTH + 1], Player player) {
    char feedback[WORD_LENGTH + 1] = "";
    char* guess;
    int round = 0;
    int won = 0;
    
    while (round < MAX_ROUNDS) {
        guess = player(feedback);
        if (!guess) {
            continue;
        }
        
        // 修改这部分
        char* new_feedback = checkWord((char*)solution, guess);
        if (!new_feedback) {
            printf("Error: Memory allocation failed\n");
            break;
        }
        strcpy(feedback, new_feedback);
        free(new_feedback);  // 释放动态分配的内存
        
        printf("You guessed: %s\n", guess);
        
        if (strcmp(feedback, "GGGGG") == 0) {
            printf("\nCongratulations! You've won in %d attempts!\n", round + 1);
            won = 1;
            break;
        }
        
        round++;
        if (round == MAX_ROUNDS) {
            printf("\nOut of attempts!\n");
        } else {
            printf("You have %d attempts remaining.\n", MAX_ROUNDS - round);
        }
    }
    
    if (!won) {
        printf("\nGame over! The word was: %s\n", solution);
    }
}

char* get_random_word(void) {
    if (wordCount == 0) return NULL;
    return wordList[rand() % wordCount];
}

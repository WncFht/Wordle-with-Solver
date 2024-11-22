// wordle.c
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "wordle.h"


// 增加词表大小
char word_list[MAX_WORDS][WORD_LENGTH + 1];
int word_count = 0;

int load_word_list(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;  
    
    word_count = 0;
    char word[100];
    
    while (fgets(word, sizeof(word), fp) && word_count < MAX_WORDS) {
        // Remove newline and convert to uppercase
        word[strcspn(word, "\n")] = 0;
        if (strlen(word) == WORD_LENGTH) {
            for (int i = 0; word[i]; i++) {
                word_list[word_count][i] = toupper(word[i]);
            }
            word_list[word_count][WORD_LENGTH] = '\0';
            word_count++;
        }
    }
    
    fclose(fp);
    return word_count;
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
    if (word_count == 0) return NULL;
    return word_list[rand() % word_count];
}

// players.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wordle.h"

char* player_input(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    printf("Enter your guess: ");
    if (scanf("%s", guess) != 1) return NULL;
    
    // 转换为大写
    for (int i = 0; guess[i]; i++) {
        guess[i] = toupper(guess[i]);
    }
    return guess;
}

char* player_random(const char lastResult[WORD_LENGTH + 1]) {
    return get_random_word();
}
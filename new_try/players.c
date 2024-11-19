// players.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "wordle.h"

// Buffer for storing the human player's input
static char input_buffer[WORD_LENGTH + 1];

// Convert input to uppercase
static void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Human player implementation
char* player_input(const char lastResult[WORD_LENGTH + 1]) {
    static char input_buffer[WORD_LENGTH * 2];  // 增大缓冲区
    
    // Print feedback from last guess if available
    if (lastResult && *lastResult) {
        printf("Feedback: %s\n", lastResult);
    }
    
    while (1) {  // 循环直到获得有效输入
        printf("Enter your guess: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            continue;  // 重试输入
        }
        
        // Remove newline and any extra characters
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(input_buffer) == 0) {
            continue;
        }
        
        // Convert to uppercase
        for (int i = 0; input_buffer[i]; i++) {
            input_buffer[i] = toupper(input_buffer[i]);
        }
        
        // 验证输入长度
        if (strlen(input_buffer) != WORD_LENGTH) {
            printf("Please enter a %d-letter word.\n", WORD_LENGTH);
            continue;  // 重试输入
        }
        
        // 验证是否都是字母
        int valid = 1;
        for (int i = 0; input_buffer[i]; i++) {
            if (!isalpha(input_buffer[i])) {
                printf("Please enter only letters.\n");
                valid = 0;
                break;
            }
        }
        if (!valid) continue;  // 重试输入
        
        // 验证是否是有效单词
        if (!is_valid_word(input_buffer)) {
            printf("Word not in dictionary. Try again.\n");
            continue;  // 重试输入
        }
        
        return input_buffer;  // 只有在输入有效时才返回
    }
}


// Random player implementation
char* player_random(const char lastResult[WORD_LENGTH + 1]) {
    static char random_guess[WORD_LENGTH + 1];
    char* word = get_random_word();
    
    if (word) {
        strncpy(random_guess, word, WORD_LENGTH);
        random_guess[WORD_LENGTH] = '\0';
        return random_guess;
    }
    
    return NULL;
}
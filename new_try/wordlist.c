// wordlist.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wordle.h"

// 增加词表大小
#define MAX_WORDS 15000
static char word_list[MAX_WORDS][WORD_LENGTH + 1];
static int word_count = 0;

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

int is_valid_word(const char* word) {
    if (strlen(word) != WORD_LENGTH) return 0;
    
    char upper_word[WORD_LENGTH + 1];
    strcpy(upper_word, word);
    for (int i = 0; upper_word[i]; i++) {
        upper_word[i] = toupper(upper_word[i]);
    }
    
    for (int i = 0; i < word_count; i++) {
        if (strcmp(word_list[i], upper_word) == 0) {
            return 1;
        }
    }
    return 0;
}

char* get_random_word(void) {
    if (word_count == 0) return NULL;
    return word_list[rand() % word_count];
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WORD_LENGTH 5

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

int main() {
    char solution[WORD_LENGTH + 1];
    char guess[WORD_LENGTH + 1];
    scanf("%s", solution);
    scanf("%s", guess);
    char* result = checkWord(solution, guess);
    printf("%s\n", result);
    free(result);
    return 0;
}
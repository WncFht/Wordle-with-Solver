#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WORD_LENGTH 5

char* checkWord(char solution[WORD_LENGTH + 1], char guess[WORD_LENGTH + 1]) {
    char* result = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
    // Fill your logic here
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
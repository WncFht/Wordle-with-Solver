#include "wordle.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int main() {
    char solution[WORD_LENGTH + 1];
    scanf("%s", solution);
    // 转换为大写
    for(int i = 0; solution[i]; i++) {
        solution[i] = toupper(solution[i]);
    }
    Player player = player_AI2;
    wordle(solution, &player);
    return 0;
}
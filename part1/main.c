// main.c
#include "wordle.h"

void printInstructions() {
    printf("\n=== Welcome to Wordle ===\n");
    printf("Try to guess the 5-letter word. You have 6 attempts.\n");
    printf("After each guess, you'll get feedback:\n");
    printf("G = Correct letter in correct position\n");
    printf("Y = Correct letter in wrong position\n");
    printf("B = Letter not in the word\n");
    printf("All input should be in UPPERCASE letters.\n\n");
}

// 清理输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 验证输入是否都是字母
int isAllLetters(const char* str) {
    for (int i = 0; str[i]; i++) {
        if (!isalpha(str[i])) return 0;
    }
    return 1;
}

int main() {
    WordleGame game;
    char guess[WORD_LENGTH + 2];  // +2 为了存储换行符和结束符
    char feedback[WORD_LENGTH + 1];
    int gameWon = 0;
    
    // 初始化游戏
    WordleStatus status = wordle_init(&game, "wordList.txt");
    if (status != WORDLE_SUCCESS) {
        printf("Error: Could not initialize game. Make sure wordList.txt exists.\n");
        return 1;
    }
    
    // 设置随机解决方案
    wordle_set_solution(&game, NULL);
    
    printInstructions();
    
    // 游戏主循环
    while (game.remainingGuesses > 0) {
        printf("\nYou have %d guesses remaining.\n", game.remainingGuesses);
        printf("Enter your guess: ");
        
        if (fgets(guess, sizeof(guess), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        
        // 移除换行符
        size_t len = strlen(guess);
        if (len > 0 && guess[len-1] == '\n') {
            guess[len-1] = '\0';
            len--;
        }
        
        // 输入验证
        if (len != WORD_LENGTH) {
            printf("Please enter exactly 5 letters.\n");
            continue;
        }
        
        if (!isAllLetters(guess)) {
            printf("Please enter only letters.\n");
            continue;
        }
        
        // 转换为大写
        for (int i = 0; guess[i]; i++) {
            guess[i] = toupper(guess[i]);
        }
        
        // 尝试猜测
        status = wordle_make_guess(&game, guess, feedback);
        
        switch (status) {
            case WORDLE_SUCCESS:
                printf("Feedback: %s\n", feedback);
                // 检查是否获胜
                if (strcmp(feedback, "GGGGG") == 0) {
                    printf("\nCongratulations! You've won in %d attempts!\n", 
                           MAX_GUESSES - game.remainingGuesses);
                    gameWon = 1;
                    game.remainingGuesses = 0; // 结束游戏
                }
                break;
                
            case WORDLE_INVALID_WORD:
                printf("Word not in list. Please try again.\n");
                game.remainingGuesses++; // 不计入尝试次数
                break;
                
            default:
                printf("Invalid input. Please try again.\n");
                game.remainingGuesses++; // 不计入尝试次数
                break;
        }
    }
    
    // 如果没有赢，显示答案
    if (!gameWon) {
        printf("\nGame Over! The word was: %s\n", game.currentSolution);
    }
    
    // 清理游戏资源
    wordle_cleanup(&game);
    
    return 0;
}
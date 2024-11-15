// wordle.c
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "wordle.h"

void generate_feedback(const char* solution, const char* guess, char* feedback) {
    int used[WORD_LENGTH] = {0};  // 用于追踪solution中已匹配的字母
    
    // 第一轮：检查完全匹配的字母（G）
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == solution[i]) {
            feedback[i] = 'G';
            used[i] = 1;
        } else {
            feedback[i] = 'B';  // 初始化为不匹配
        }
    }
    
    // 第二轮：检查存在但位置错误的字母（Y）
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == 'B') {  // 只检查第一轮标记为不匹配的位置
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (!used[j] && guess[i] == solution[j]) {
                    feedback[i] = 'Y';
                    used[j] = 1;  // 标记这个solution字母已被匹配
                    break;
                }
            }
        }
    }
    
    feedback[WORD_LENGTH] = '\0';  // 确保字符串正确终止
}

void wordle(const char solution[WORD_LENGTH + 1], Player player) {
    char feedback[WORD_LENGTH + 1] = "";
    char* guess;
    int round = 0;
    int won = 0;
    
    while (round < MAX_ROUNDS) {
        // 获取玩家猜测
        guess = player(feedback);
        if (!guess) {
            continue;  // 无效输入时继续尝试，而不是break
        }
        
        // 生成反馈
        generate_feedback(solution, guess, feedback);
        printf("You guessed: %s\n", guess);  // 显示玩家的猜测
        
        // 检查是否获胜
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
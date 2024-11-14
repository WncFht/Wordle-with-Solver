// wordle.h
#ifndef WORDLE_H
#define WORDLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define WORD_LENGTH 5
#define MAX_GUESSES 6
#define MAX_WORDS 15000
#define SOLUTION_WORDS 2315

// 游戏状态枚举
typedef enum {
    WORDLE_SUCCESS,
    WORDLE_INVALID_WORD,
    WORDLE_FILE_ERROR,
    WORDLE_MEMORY_ERROR,
    WORDLE_INVALID_INPUT
} WordleStatus;

// Wordle游戏结构
typedef struct {
    char** wordList;        // 所有合法单词
    int wordCount;          // 单词总数
    char** solutions;       // 可能的解决方案
    int solutionCount;      // 解决方案数量
    char currentSolution[WORD_LENGTH + 1];  // 当前游戏的解决方案
    int remainingGuesses;   // 剩余猜测次数
} WordleGame;

// 初始化游戏
WordleStatus wordle_init(WordleGame* game, const char* wordListPath);

// 设置新的解决方案（随机或指定）
WordleStatus wordle_set_solution(WordleGame* game, const char* solution);

// 验证猜测并生成反馈
WordleStatus wordle_make_guess(WordleGame* game, const char* guess, char* feedback);

// 检查单词是否合法
int wordle_is_valid_word(WordleGame* game, const char* word);

// 清理游戏资源
void wordle_cleanup(WordleGame* game);

#endif // WORDLE_H
// wordle.h
#ifndef WORDLE_H
#define WORDLE_H

#include <stdbool.h>

#define WORD_LENGTH 5
#define MAX_ROUNDS 10
#define MAX_WORDS 15000
#define SOLUTION_LENGTH 2500

typedef char* (*Player)(const char lastResult[WORD_LENGTH + 1]);

// 添加全局变量声明
extern char wordList[MAX_WORDS][WORD_LENGTH + 1];
extern int wordCount;
extern char solutionList[SOLUTION_LENGTH][WORD_LENGTH + 1];
extern int solutionCount;

// 添加函数声明
void loadWords(const char* filename);
void loadSolution(const char* filename);
char* checkWord(const char solution[WORD_LENGTH + 1], const char guess[WORD_LENGTH + 1]);
void wordle(const char solution[WORD_LENGTH + 1], Player *player);
bool not_legal(const char* guess);
bool invalid(const char* solution);

#endif
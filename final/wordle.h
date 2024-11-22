// wordle.h
#ifndef WORDLE_H
#define WORDLE_H

#define WORD_LENGTH 5
#define MAX_WORDS 15000
#define MAX_ROUNDS 6

// Player function type - takes last result, returns next guess
typedef char* (*Player)(const char lastResult[WORD_LENGTH + 1]);

// 全局变量声明
extern char wordList[MAX_WORDS][WORD_LENGTH + 1];
extern char solutions[MAX_WORDS][WORD_LENGTH + 1];
// extern char* possibleSolutions[MAX_WORDS];
// extern int numWords;
// extern int numSolutions;
// extern int numPossible;
// extern int firstGuess; 

// 函数声明
void generate_feedback(const char* solution, const char* guess, char* feedback);
// void updatePossibleSolutions(const char* guess, const char* feedback);
// void getPattern(const char* guess, const char* target, char* pattern);
// void initGame(int gameMode);
// char* findBestGuess(int remainingAttempts);
// void loadWords();
// void calculateLetterWeights();
// void precomputeWordMasks();
// void precomputePatterns();
// void runAlgorithmTest();
int load_word_list(const char* filename);
int is_valid_word(const char* word);
char* get_random_word(void);

// 核心游戏函数
void wordle(const char solution[WORD_LENGTH + 1], Player player);

// 示例玩家实现
char* player_input(const char lastResult[WORD_LENGTH + 1]);  // 人类玩家
char* player_random(const char lastResult[WORD_LENGTH + 1]); // 随机猜测
char* player_AI(const char lastResult[WORD_LENGTH + 1]); 
char* player_entropy(const char lastResult[WORD_LENGTH + 1]); 
char* player_minimax(const char lastResult[WORD_LENGTH + 1]);
char* player_frequency(const char lastResult[WORD_LENGTH + 1]);

#endif // WORDLE_H
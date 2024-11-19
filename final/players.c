// players.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "wordle.h"

#define MAX_WORDS 15000
#define PATTERN_COUNT 243  // 3^5 可能的反馈模式数量

// 函数前向声明
static void to_uppercase(char* str);
static void init_possible_solutions(void);
static void update_solutions(const char* guess, const char* feedback);
void cleanup_ai(void);
static float calculate_entropy(const char* word, int* pattern_counts);
static void generate_pattern_counts(const char* word, int* pattern_counts);

// 全局变量
static char input_buffer[WORD_LENGTH + 1];
static char** possible_solutions = NULL;
static int solution_count = 0;
static char last_guess[WORD_LENGTH + 1] = {0};
static char best_guess_buffer[WORD_LENGTH + 1] = {0};
extern char word_list[MAX_WORDS][WORD_LENGTH + 1];
extern int word_count;

// 辅助函数实现
static void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

static void init_possible_solutions(void) {
    if (possible_solutions != NULL) {
        cleanup_ai();
    }
    
    possible_solutions = (char**)malloc(MAX_WORDS * sizeof(char*));
    if (!possible_solutions) return;
    
    for (int i = 0; i < MAX_WORDS; i++) {
        possible_solutions[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!possible_solutions[i]) {
            // 清理已分配的内存
            for (int j = 0; j < i; j++) {
                free(possible_solutions[j]);
            }
            free(possible_solutions);
            possible_solutions = NULL;
            return;
        }
    }
    
    for (int i = 0; i < word_count; i++) {
        strcpy(possible_solutions[i], word_list[i]);
    }
    solution_count = word_count;
}

static void update_solutions(const char* guess, const char* feedback) {
    if (!possible_solutions || solution_count == 0) return;
    
    char** temp = (char**)malloc(solution_count * sizeof(char*));
    if (!temp) return;
    
    for (int i = 0; i < solution_count; i++) {
        temp[i] = (char*)malloc((WORD_LENGTH + 1) * sizeof(char));
        if (!temp[i]) {
            for (int j = 0; j < i; j++) free(temp[j]);
            free(temp);
            return;
        }
    }
    
    int new_count = 0;
    for (int i = 0; i < solution_count; i++) {
        char test_feedback[WORD_LENGTH + 1];
        generate_feedback(possible_solutions[i], guess, test_feedback);
        if (strcmp(test_feedback, feedback) == 0) {
            strcpy(temp[new_count++], possible_solutions[i]);
        }
    }
    
    // 更新候选列表
    for (int i = 0; i < new_count; i++) {
        strcpy(possible_solutions[i], temp[i]);
    }
    solution_count = new_count;
    
    // 清理临时空间
    for (int i = 0; i < solution_count; i++) {
        free(temp[i]);
    }
    free(temp);
}

void cleanup_ai(void) {
    if (possible_solutions) {
        for (int i = 0; i < MAX_WORDS; i++) {
            free(possible_solutions[i]);
        }
        free(possible_solutions);
        possible_solutions = NULL;
    }
    solution_count = 0;
}

static float calculate_entropy(const char* word, int* pattern_counts) {
    float entropy = 0.0f;
    for (int i = 0; i < PATTERN_COUNT; i++) {
        if (pattern_counts[i] > 0) {
            float p = (float)pattern_counts[i] / solution_count;
            entropy -= p * log2f(p);
        }
    }
    return entropy;
}

static void generate_pattern_counts(const char* word, int* pattern_counts) {
    memset(pattern_counts, 0, PATTERN_COUNT * sizeof(int));
    
    for (int i = 0; i < solution_count; i++) {
        char test_feedback[WORD_LENGTH + 1];
        generate_feedback(possible_solutions[i], word, test_feedback);
        
        int pattern_index = 0;
        for (int j = 0; j < WORD_LENGTH; j++) {
            pattern_index = pattern_index * 3 + 
                (test_feedback[j] == 'G' ? 2 : 
                 test_feedback[j] == 'Y' ? 1 : 0);
        }
        pattern_counts[pattern_index]++;
    }
}

// 玩家实现
char* player_input(const char lastResult[WORD_LENGTH + 1]) {
    static char input_buffer[WORD_LENGTH * 2];
    
    if (lastResult && *lastResult) {
        printf("Feedback: %s\n", lastResult);
    }
    
    while (1) {
        printf("Enter your guess: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            continue;
        }
        
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        if (strlen(input_buffer) == 0) continue;
        
        to_uppercase(input_buffer);
        
        if (strlen(input_buffer) != WORD_LENGTH) {
            printf("Please enter a %d-letter word.\n", WORD_LENGTH);
            continue;
        }
        
        int valid = 1;
        for (int i = 0; input_buffer[i]; i++) {
            if (!isalpha(input_buffer[i])) {
                printf("Please enter only letters.\n");
                valid = 0;
                break;
            }
        }
        if (!valid) continue;
        
        if (!is_valid_word(input_buffer)) {
            printf("Word not in dictionary. Try again.\n");
            continue;
        }
        
        return input_buffer;
    }
}

char* player_random(const char lastResult[WORD_LENGTH + 1]) {
    static char random_guess[WORD_LENGTH + 1];
    char* word = get_random_word();
    
    if (!word) return NULL;
    
    strncpy(random_guess, word, WORD_LENGTH);
    random_guess[WORD_LENGTH] = '\0';
    return random_guess;
}

char* player_entropy(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    
    // 初始化
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");  // 经验验证的最优起始词
        strcpy(last_guess, guess);
        return guess;
    }
    
    // 处理上一次猜测的结果
    if (lastResult && *lastResult) {
        update_solutions(last_guess, lastResult);
        printf("Remaining possible solutions: %d\n", solution_count);
        
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        } else if (solution_count == 0) {
            cleanup_ai();
            init_possible_solutions();
            strcpy(guess, "STARE");
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    // 分配模式计数数组
    int* pattern_counts = (int*)calloc(PATTERN_COUNT, sizeof(int));
    if (!pattern_counts) {
        strcpy(guess, possible_solutions[0]);
        strcpy(last_guess, guess);
        return guess;
    }
    
    // 计算最佳猜测
    float max_entropy = -1.0f;
    for (int i = 0; i < word_count; i++) {
        generate_pattern_counts(word_list[i], pattern_counts);
        float entropy = calculate_entropy(word_list[i], pattern_counts);
        
        if (entropy > max_entropy) {
            max_entropy = entropy;
            strcpy(best_guess_buffer, word_list[i]);
        }
    }
    
    free(pattern_counts);
    
    // 选择最终猜测
    if (max_entropy > -1.0f) {
        strcpy(guess, best_guess_buffer);
    } else {
        strcpy(guess, possible_solutions[0]);
    }
    
    strcpy(last_guess, guess);
    printf("Selected guess: %s (entropy: %.2f)\n", guess, max_entropy);
    return guess;
}

char* player_minimax(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    static char best_guess[WORD_LENGTH + 1];
    
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");
        strcpy(last_guess, guess);
        return guess;
    }
    
    if (lastResult && *lastResult && strlen(last_guess) > 0) {
        update_solutions(last_guess, lastResult);
        
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    int min_worst_case = MAX_WORDS;
    int* pattern_counts = (int*)calloc(PATTERN_COUNT, sizeof(int));
    
    if (!pattern_counts) {
        strcpy(guess, possible_solutions[0]);
        strcpy(last_guess, guess);
        return guess;
    }
    
    for (int i = 0; i < word_count; i++) {
        generate_pattern_counts(word_list[i], pattern_counts);
        
        int max_remaining = 0;
        for (int j = 0; j < PATTERN_COUNT; j++) {
            if (pattern_counts[j] > max_remaining) {
                max_remaining = pattern_counts[j];
            }
        }
        
        if (max_remaining < min_worst_case) {
            min_worst_case = max_remaining;
            strcpy(best_guess, word_list[i]);
        }
    }
    
    free(pattern_counts);
    strcpy(guess, best_guess);
    strcpy(last_guess, guess);
    return guess;
}

char* player_frequency(const char lastResult[WORD_LENGTH + 1]) {
    static char guess[WORD_LENGTH + 1];
    static char best_guess[WORD_LENGTH + 1];
    
    // 初始化候选列表
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");  // 首次猜测
        strcpy(last_guess, guess);
        return guess;
    }
    
    // 处理上一次猜测的结果
    if (lastResult && *lastResult && strlen(last_guess) > 0) {
        update_solutions(last_guess, lastResult);
        printf("Remaining possible solutions: %d\n", solution_count);
        
        // 如果只剩一个答案，直接返回
        if (solution_count == 1) {
            strcpy(guess, possible_solutions[0]);
            strcpy(last_guess, guess);
            return guess;
        } else if (solution_count == 0) {
            // 如果没有可能的解，重新初始化
            cleanup_ai();
            init_possible_solutions();
            strcpy(guess, "STARE");
            strcpy(last_guess, guess);
            return guess;
        }
    }
    
    // 计算字母频率
    float letter_freq[26][WORD_LENGTH] = {0};  // 每个位置的字母频率
    float total_freq[26] = {0};                // 总体字母频率
    
    // 统计当前可能解中的字母频率
    for (int i = 0; i < solution_count; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            int letter = possible_solutions[i][j] - 'A';
            letter_freq[letter][j]++;
            total_freq[letter]++;
        }
    }
    
    // 归一化频率
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            letter_freq[i][j] /= solution_count;
        }
        total_freq[i] /= (solution_count * WORD_LENGTH);
    }
    
    // 评分并选择最佳猜测
    float best_score = -1.0f;
    
    // 考虑所有可能的单词
    for (int i = 0; i < word_count; i++) {
        float score = 0.0f;
        int used[26] = {0};
        
        // 计算位置得分
        for (int j = 0; j < WORD_LENGTH; j++) {
            int letter = word_list[i][j] - 'A';
            score += letter_freq[letter][j] * 2.0f;  // 位置权重加倍
            
            if (!used[letter]) {
                score += total_freq[letter];
                used[letter] = 1;
            }
        }
        
        if (score > best_score) {
            best_score = score;
            strcpy(best_guess, word_list[i]);
        }
    }
    
    // 如果找到了好的猜测
    if (best_score > -1.0f) {
        strcpy(guess, best_guess);
    } else {
        // 如果出现异常情况，选择第一个可能的解
        strcpy(guess, possible_solutions[0]);
    }
    
    strcpy(last_guess, guess);
    printf("Selected guess: %s (score: %.2f)\n", guess, best_score);
    return guess;
}
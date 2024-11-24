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
// static void to_uppercase(char* str);
static void init_possible_solutions(void);
static void update_solutions(const char* guess, const char* feedback);
void cleanup_ai(void);
static float calculate_entropy(int* pattern_counts);
static void generate_pattern_counts(const char* word, int* pattern_counts);

// 全局变量
// static char input_buffer[WORD_LENGTH + 1];
static char** possible_solutions = NULL;
static int solution_count = 0;
static char last_guess[WORD_LENGTH + 1] = {0};
static char best_guess_buffer[WORD_LENGTH + 1] = {0};
extern char wordList[MAX_WORDS][WORD_LENGTH + 1];
extern int wordCount;

// 辅助函数实现
// int is_valid_word(const char* word) {
//     if (strlen(word) != WORD_LENGTH) return 0;
    
//     char upper_word[WORD_LENGTH + 1];
//     strcpy(upper_word, word);
//     for (int i = 0; upper_word[i]; i++) {
//         upper_word[i] = toupper(upper_word[i]);
//     }
    
//     for (int i = 0; i < wordCount; i++) {
//         if (strcmp(wordList[i], upper_word) == 0) {
//             return 1;
//         }
//     }
//     return 0;
// }

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
    
    for (int i = 0; i < wordCount; i++) {
        strcpy(possible_solutions[i], wordList[i]);
    }
    solution_count = wordCount;
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
        // 修改这部分
        char* test_feedback = checkWord(possible_solutions[i], guess);
        if (!test_feedback) {
            // 处理内存分配失败
            for (int j = 0; j < solution_count; j++) free(temp[j]);
            free(temp);
            return;
        }
        if (strcmp(test_feedback, feedback) == 0) {
            strcpy(temp[new_count++], possible_solutions[i]);
        }
        free(test_feedback);  // 释放动态分配的内存
    }
    
    for (int i = 0; i < new_count; i++) {
        strcpy(possible_solutions[i], temp[i]);
    }
    solution_count = new_count;
    
    for (int i = 0; i < solution_count; i++) {
        free(temp[i]);
    }
    free(temp);
}

static void generate_pattern_counts(const char* word, int* pattern_counts) {
    memset(pattern_counts, 0, PATTERN_COUNT * sizeof(int));
    
    for (int i = 0; i < solution_count; i++) {
        // 修改这部分
        char* test_feedback = checkWord(possible_solutions[i], word);
        if (!test_feedback) continue;  // 处理内存分配失败
        
        int pattern_index = 0;
        for (int j = 0; j < WORD_LENGTH; j++) {
            pattern_index = pattern_index * 3 + 
                (test_feedback[j] == 'G' ? 2 : 
                 test_feedback[j] == 'Y' ? 1 : 0);
        }
        pattern_counts[pattern_index]++;
        free(test_feedback);  // 释放动态分配的内存
    }
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

static float calculate_entropy(int* pattern_counts) {
    float entropy = 0.0f;
    for (int i = 0; i < PATTERN_COUNT; i++) {
        if (pattern_counts[i] > 0) {
            float p = (float)pattern_counts[i] / solution_count;
            entropy -= p * log2f(p);
        }
    }
    return entropy;
}

// 玩家实现
// char* player_input(const char lastResult[WORD_LENGTH + 1]) {
//     static char input_buffer[WORD_LENGTH * 2];
    
//     if (lastResult && *lastResult) {
//         printf("Feedback: %s\n", lastResult);
//     }
    
//     while (1) {
//         printf("Enter your guess: ");
//         if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
//             continue;
//         }
        
//         input_buffer[strcspn(input_buffer, "\n")] = 0;
//         if (strlen(input_buffer) == 0) continue;
        
//         to_uppercase(input_buffer);
        
//         if (strlen(input_buffer) != WORD_LENGTH) {
//             printf("Please enter a %d-letter word.\n", WORD_LENGTH);
//             continue;
//         }
        
//         int valid = 1;
//         for (int i = 0; input_buffer[i]; i++) {
//             if (!isalpha(input_buffer[i])) {
//                 printf("Please enter only letters.\n");
//                 valid = 0;
//                 break;
//             }
//         }
//         if (!valid) continue;
        
//         if (!is_valid_word(input_buffer)) {
//             printf("Word not in dictionary. Try again.\n");
//             continue;
//         }
        
//         return input_buffer;
//     }
// }

// char* player_random(const char lastResult[WORD_LENGTH + 1]) {
//     static char random_guess[WORD_LENGTH + 1];
//     char* word = get_random_word();
    
//     if (!word) return NULL;
    
//     strncpy(random_guess, word, WORD_LENGTH);
//     random_guess[WORD_LENGTH] = '\0';
//     return random_guess;
// }

char* player_AI1(const char lastResult[WORD_LENGTH + 1]) {
    char* guess = (char*)malloc(WORD_LENGTH + 1);
    if (!guess) return NULL;
    
    printf("Last result: %s\n", lastResult);
    // 初始化
    if (!possible_solutions) {
        init_possible_solutions();
        strcpy(guess, "STARE");  // 使用 STARE 作为固定的起始词
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
            // 不直接返回 NULL，而是重新初始化
            cleanup_ai();
            init_possible_solutions();
            strcpy(guess, "STARE");  // 重新从起始词开始
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
    
    float max_entropy = -1.0f;
    const char* best_word = NULL;
    
    if (solution_count <= 2) {
        // 当剩余可能解很少时，直接从中选择
        strcpy(guess, possible_solutions[0]);
    } else {
        // 遍历所有单词寻找最优猜测
        for (int i = 0; i < wordCount; i++) {
            generate_pattern_counts(wordList[i], pattern_counts);
            float entropy = calculate_entropy(pattern_counts);
            
            // 优化：考虑字母位置的匹配情况
            if (lastResult && *lastResult) {
                float position_bonus = 0.0f;
                for (int j = 0; j < WORD_LENGTH; j++) {
                    if (lastResult[j] == 'G' && wordList[i][j] == last_guess[j]) {
                        position_bonus += 0.3f;  // 已确认位置的权重
                    }
                    if (lastResult[j] == 'Y' && wordList[i][j] != last_guess[j]) {
                        for (int k = 0; k < WORD_LENGTH; k++) {
                            if (k != j && wordList[i][k] == last_guess[j]) {
                                position_bonus += 0.2f;  // 已知字母但位置不同的权重
                                break;
                            }
                        }
                    }
                }
                entropy += position_bonus;
            }
            
            // 如果该词在可能解中，给予额外权重
            for (int j = 0; j < solution_count; j++) {
                if (strcmp(wordList[i], possible_solutions[j]) == 0) {
                    entropy += 0.1f;  // 可能解的额外权重
                    break;
                }
            }
            
            if (entropy > max_entropy) {
                max_entropy = entropy;
                best_word = wordList[i];
            }
        }
        
        if (best_word) {
            strcpy(guess, best_word);
        } else {
            strcpy(guess, possible_solutions[0]);
        }
    }
    
    free(pattern_counts);
    
    strcpy(last_guess, guess);
    printf("Selected guess: %s (entropy: %.2f)\n", guess, max_entropy);
    return guess;
}
// char* player_AI3(const char lastResult[WORD_LENGTH + 1]) {
//     static char guess[WORD_LENGTH + 1];
//     static char best_guess[WORD_LENGTH + 1];
    
//     if (!possible_solutions) {
//         init_possible_solutions();
//         strcpy(guess, "STARE");
//         strcpy(last_guess, guess);
//         return guess;
//     }
    
//     if (lastResult && *lastResult && strlen(last_guess) > 0) {
//         update_solutions(last_guess, lastResult);
        
//         if (solution_count == 1) {
//             strcpy(guess, possible_solutions[0]);
//             strcpy(last_guess, guess);
//             return guess;
//         }
//     }
    
//     int min_worst_case = MAX_WORDS;
//     int* pattern_counts = (int*)calloc(PATTERN_COUNT, sizeof(int));
    
//     if (!pattern_counts) {
//         strcpy(guess, possible_solutions[0]);
//         strcpy(last_guess, guess);
//         return guess;
//     }
    
//     for (int i = 0; i < wordCount; i++) {
//         generate_pattern_counts(wordList[i], pattern_counts);
        
//         int max_remaining = 0;
//         for (int j = 0; j < PATTERN_COUNT; j++) {
//             if (pattern_counts[j] > max_remaining) {
//                 max_remaining = pattern_counts[j];
//             }
//         }
        
//         if (max_remaining < min_worst_case) {
//             min_worst_case = max_remaining;
//             strcpy(best_guess, wordList[i]);
//         }
//     }
    
//     free(pattern_counts);
//     strcpy(guess, best_guess);
//     strcpy(last_guess, guess);
//     return guess;
// }

// char* player_frequency(const char lastResult[WORD_LENGTH + 1]) {
//     static char guess[WORD_LENGTH + 1];
//     static char best_guess[WORD_LENGTH + 1];
    
//     // 初始化候选列表
//     if (!possible_solutions) {
//         init_possible_solutions();
//         strcpy(guess, "STARE");  // 首次猜测
//         strcpy(last_guess, guess);
//         return guess;
//     }
    
//     // 处理上一次猜测的结果
//     if (lastResult && *lastResult && strlen(last_guess) > 0) {
//         update_solutions(last_guess, lastResult);
//         printf("Remaining possible solutions: %d\n", solution_count);
        
//         // 如果只剩一个答案，直接返回
//         if (solution_count == 1) {
//             strcpy(guess, possible_solutions[0]);
//             strcpy(last_guess, guess);
//             return guess;
//         } else if (solution_count == 0) {
//             // 如果没有可能的解，重新初始化
//             cleanup_ai();
//             init_possible_solutions();
//             strcpy(guess, "STARE");
//             strcpy(last_guess, guess);
//             return guess;
//         }
//     }
    
//     // 计算字母频率
//     float letter_freq[26][WORD_LENGTH] = {0};  // 每个位置的字母频率
//     float total_freq[26] = {0};                // 总体字母频率
    
//     // 统计当前可能解中的字母频率
//     for (int i = 0; i < solution_count; i++) {
//         for (int j = 0; j < WORD_LENGTH; j++) {
//             int letter = possible_solutions[i][j] - 'A';
//             letter_freq[letter][j]++;
//             total_freq[letter]++;
//         }
//     }
    
//     // 归一化频率
//     for (int i = 0; i < 26; i++) {
//         for (int j = 0; j < WORD_LENGTH; j++) {
//             letter_freq[i][j] /= solution_count;
//         }
//         total_freq[i] /= (solution_count * WORD_LENGTH);
//     }
    
//     // 评分并选择最佳猜测
//     float best_score = -1.0f;
    
//     // 考虑所有可能的单词
//     for (int i = 0; i < wordCount; i++) {
//         float score = 0.0f;
//         int used[26] = {0};
        
//         // 计算位置得分
//         for (int j = 0; j < WORD_LENGTH; j++) {
//             int letter = wordList[i][j] - 'A';
//             score += letter_freq[letter][j] * 2.0f;  // 位置权重加倍
            
//             if (!used[letter]) {
//                 score += total_freq[letter];
//                 used[letter] = 1;
//             }
//         }
        
//         if (score > best_score) {
//             best_score = score;
//             strcpy(best_guess, wordList[i]);
//         }
//     }
    
//     // 如果找到了好的猜测
//     if (best_score > -1.0f) {
//         strcpy(guess, best_guess);
//     } else {
//         // 如果出现异常情况，选择第一个可能的解
//         strcpy(guess, possible_solutions[0]);
//     }
    
//     strcpy(last_guess, guess);
//     printf("Selected guess: %s (score: %.2f)\n", guess, best_score);
//     return guess;
// }

#define MAX_LINE_LENGTH 256
#define MAX_LINES 10000

static char** decision_lines = NULL;
static int line_count = 0;
static char current_word[6] = "SALET";
static bool first_guess = true;
static char cumulative_pattern[MAX_LINE_LENGTH] = "";  // 累积的pattern串

static void load_decision_tree(void) {
    static bool loaded = false;
    if (loaded) return;

    FILE* file = fopen("tree.txt", "r");
    if (!file) {
        printf("Failed to open tree.txt\n");
        return;
    }

    decision_lines = (char**)malloc(MAX_LINES * sizeof(char*));
    if (!decision_lines) {
        fclose(file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && line_count < MAX_LINES) {
        line[strcspn(line, "\n")] = 0;
        
        decision_lines[line_count] = strdup(line);
        if (!decision_lines[line_count]) {
            for (int i = 0; i < line_count; i++) {
                free(decision_lines[i]);
            }
            free(decision_lines);
            decision_lines = NULL;
            fclose(file);
            return;
        }
        
        // printf("Loaded line %d: %s\n", line_count, decision_lines[line_count]);
        line_count++;
    }

    fclose(file);
    loaded = true;
    printf("Loaded %d decision lines\n", line_count);
}

static const char* find_next_move(const char* feedback, int level) {
    // 在累积pattern末尾追加新pattern
    char new_pattern[32];
    sprintf(new_pattern, "%s %s%d ", current_word, feedback, level);
    strcat(cumulative_pattern, new_pattern);
    
    printf("Looking for pattern: '%s'\n", cumulative_pattern);
    
    // 查找匹配的行
    for (int i = 0; i < line_count; i++) {
        // printf("Comparing with line %d: %s\n", i, decision_lines[i]);
        if (strstr(decision_lines[i], cumulative_pattern) == decision_lines[i]) {
            // 提取下一个单词
            const char* line = decision_lines[i] + strlen(cumulative_pattern);
            char next_word[WORD_LENGTH + 1];
            if (sscanf(line, "%5s", next_word) == 1) {
                printf("Found next word: %s in line: %s\n", next_word, decision_lines[i]);
                return strdup(next_word);
            }
        }
    }
    
    printf("No matching move found\n");
    return NULL;
}

char* player_AI2(const char lastResult[WORD_LENGTH + 1]) {
    load_decision_tree();
    // printf("Last result: %s\n", lastResult);
    char* guess = malloc(WORD_LENGTH + 1);
    if (!guess) return NULL;

    bool is_first_guess = true;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (lastResult[i] != ' ') {
            is_first_guess = false;
            break;
        }
    }

    if (is_first_guess) {
        // printf("First guess: SALET\n");
        strcpy(guess, "SALET");
        strcpy(current_word, "SALET");
        cumulative_pattern[0] = '\0';  // 清空累积pattern
        return guess;
    }

    printf("Current word: %s\n", current_word);
    // printf("Last result: %s\n", lastResult);

    // 通过累积pattern的长度/状态计算level
    int level = 1;
    for (char* p = cumulative_pattern; *p; p++) {
        if (*p >= '1' && *p <= '9') level++;
    }

    const char* next = find_next_move(lastResult, level);
    if (next) {
        strcpy(guess, next);
        strcpy(current_word, next);
        free((void*)next);
        return guess;
    }

    free(guess);
    return NULL;
}
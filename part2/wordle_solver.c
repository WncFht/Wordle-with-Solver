
// wordle_solver.c
#include "wordle_solver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WORDS 15000

// 存储单词和状态
static char all_words[MAX_WORDS][WORD_LENGTH + 1];
static char possible_words[MAX_WORDS][WORD_LENGTH + 1];
static int word_count = 0;
static int possible_count = 0;
static char previous_guesses[6][WORD_LENGTH + 1];
static int guess_count = 0;

// 最佳初始单词列表
static const char* FIRST_WORDS[] = {
    "SALET", "CRANE", "TRACE", "SLATE", "CRATE"
};

// 最佳第二轮单词列表（根据第一轮反馈选择）
static const char* SECOND_WORDS[] = {
    "ROUND", "PILOT", "BUNCH", "CLOUD", "HUMID"
};

// 计算词中字母的频率
static void get_letter_freq(double freq[26]) {
    memset(freq, 0, sizeof(double) * 26);
    for(int i = 0; i < possible_count; i++) {
        for(int j = 0; j < WORD_LENGTH; j++) {
            freq[possible_words[i][j] - 'A'] += 1.0 / possible_count;
        }
    }
}

// 检查是否已经猜过这个词
static int is_word_guessed(const char* word) {
    for(int i = 0; i < guess_count; i++) {
        if(strcmp(previous_guesses[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

// 计算两个单词的匹配模式
static void get_pattern(const char* word, const char* guess, char* pattern) {
    int letter_count[26] = {0};
    int used[WORD_LENGTH] = {0};
    
    // 统计字母出现次数
    for(int i = 0; i < WORD_LENGTH; i++) {
        letter_count[word[i] - 'A']++;
    }
    
    // 标记绿色(G)
    for(int i = 0; i < WORD_LENGTH; i++) {
        if(guess[i] == word[i]) {
            pattern[i] = 'G';
            used[i] = 1;
            letter_count[guess[i] - 'A']--;
        } else {
            pattern[i] = 'B';
        }
    }
    
    // 标记黄色(Y)
    for(int i = 0; i < WORD_LENGTH; i++) {
        if(!used[i] && letter_count[guess[i] - 'A'] > 0) {
            pattern[i] = 'Y';
            letter_count[guess[i] - 'A']--;
        }
    }
    pattern[WORD_LENGTH] = '\0';
}

// 更新可能的单词列表
static void update_possible_words(const char* guess, const char* feedback) {
    char pattern[WORD_LENGTH + 1];
    int new_count = 0;
    
    for(int i = 0; i < possible_count; i++) {
        get_pattern(possible_words[i], guess, pattern);
        if(strcmp(pattern, feedback) == 0) {
            if(new_count != i) {
                strcpy(possible_words[new_count], possible_words[i]);
            }
            new_count++;
        }
    }
    possible_count = new_count;
}

// 计算单词的得分
static double evaluate_word(const char* word, const char* feedback, double letter_freq[26]) {
    // 如果单词已经猜过，返回最低分
    if(is_word_guessed(word)) {
        return -1000.0;
    }
    
    int positions[26][WORD_LENGTH] = {0};  // 记录每个字母在每个位置的出现次数
    double position_scores[WORD_LENGTH] = {0.0};  // 每个位置的得分
    
    // 统计位置信息
    for(int i = 0; i < possible_count; i++) {
        for(int j = 0; j < WORD_LENGTH; j++) {
            positions[possible_words[i][j] - 'A'][j]++;
        }
    }
    
    // 计算位置得分
    for(int i = 0; i < WORD_LENGTH; i++) {
        position_scores[i] = (double)positions[word[i] - 'A'][i] / possible_count;
    }
    
    // 计算总得分
    double score = 0.0;
    int used_letters[26] = {0};
    int unique_letters = 0;
    
    for(int i = 0; i < WORD_LENGTH; i++) {
        int letter = word[i] - 'A';
        score += letter_freq[letter] * (1.0 + position_scores[i]);
        if(!used_letters[letter]) {
            used_letters[letter] = 1;
            unique_letters++;
        }
    }
    
    // 奖励唯一字母
    score *= (1.0 + 0.2 * unique_letters);
    
    // 如果是可能的答案，增加得分
    for(int i = 0; i < possible_count; i++) {
        if(strcmp(word, possible_words[i]) == 0) {
            score *= 1.5;
            break;
        }
    }
    
    return score;
}

// 获取最佳猜测
static char* get_best_guess(const char* feedback) {
    static char best_word[WORD_LENGTH + 1];
    double best_score = -1000.0;
    double letter_freq[26];
    
    // 第一次猜测使用预定义的最佳起始词
    if(!feedback || strcmp(feedback, "BBBBB") == 0) {
        strcpy(best_word, FIRST_WORDS[guess_count % 5]);
        return best_word;
    }
    
    // 如果只剩一个可能答案，直接返回
    if(possible_count == 1) {
        strcpy(best_word, possible_words[0]);
        return best_word;
    }
    
    // 如果是第二次猜测，使用预定义的第二轮单词
    if(guess_count == 1) {
        // 根据第一轮反馈选择合适的第二轮单词
        for(int i = 0; i < 5; i++) {
            if(!is_word_guessed(SECOND_WORDS[i])) {
                strcpy(best_word, SECOND_WORDS[i]);
                return best_word;
            }
        }
    }
    
    // 计算当前字母频率
    get_letter_freq(letter_freq);
    
    // 如果剩余可能答案较少，只从可能答案中选择
    if(possible_count <= 10) {
        for(int i = 0; i < possible_count; i++) {
            double score = evaluate_word(possible_words[i], feedback, letter_freq);
            if(score > best_score) {
                best_score = score;
                strcpy(best_word, possible_words[i]);
            }
        }
        return best_word;
    }
    
    // 评估所有单词
    #pragma omp parallel for
    for(int i = 0; i < word_count; i++) {
        double score = evaluate_word(all_words[i], feedback, letter_freq);
        #pragma omp critical
        {
            if(score > best_score) {
                best_score = score;
                strcpy(best_word, all_words[i]);
            }
        }
    }
    
    return best_word;
}

void init_solver(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if(!fp) return;
    
    word_count = 0;
    char word[100];
    
    while(fgets(word, sizeof(word), fp) && word_count < MAX_WORDS) {
        word[strcspn(word, "\n")] = 0;
        if(strlen(word) == WORD_LENGTH) {
            for(int i = 0; word[i]; i++) {
                word[i] = toupper(word[i]);
            }
            strcpy(all_words[word_count], word);
            strcpy(possible_words[word_count], word);
            word_count++;
        }
    }
    possible_count = word_count;
    fclose(fp);
    
    // 初始化猜测历史
    guess_count = 0;
    memset(previous_guesses, 0, sizeof(previous_guesses));
}

char* player_optimal(const char lastResult[WORD_LENGTH + 1]) {
    char* guess = get_best_guess(lastResult);
    
    if(lastResult[0] != 'B' || lastResult[1] != 'B') {
        update_possible_words(previous_guesses[guess_count-1], lastResult);
    }
    
    strcpy(previous_guesses[guess_count], guess);
    guess_count++;
    
    return guess;
}
#include "wordle.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    char word[WORD_LENGTH + 1];
    int score;
    int guesses;
} TestResult;

// 这个数组用来捕获每次调用 wordle 的最后一行输出（得分）
int found_score = 0;
int found_guesses = 0;

// 重写一个简单版本的 wordle 函数
void test_wordle(const char solution[WORD_LENGTH + 1], Player *player) {
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    if (invalid(solution)) {
        return;
    }
    
    char* lastResult = (char*)malloc(WORD_LENGTH * sizeof(char));
    for (int i = 0; i < WORD_LENGTH; i++) {
        lastResult[i] = ' ';
    }
    lastResult[WORD_LENGTH] = '\0';
    
    int i;
    for (i = 0; i < MAX_ROUNDS; i++) {
        char* guess = (*player)(lastResult);
        if (not_legal(guess)) {
            free(guess);
            continue;
        }
        
        char* result = checkWord(solution, guess);
        if (strcmp(solution, guess) == 0) {
            found_score = 10 - i;
            found_guesses = i + 1;
            free(guess);
            free(result);
            free(lastResult);
            return;
        }
        
        free(guess);
        free(lastResult);
        lastResult = result;
    }
    
    found_score = 0;
    found_guesses = MAX_ROUNDS;
    free(lastResult);
}

int main() {
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    
    TestResult* results = (TestResult*)malloc(solutionCount * sizeof(TestResult));
    int total_score = 0;
    int total_guesses = 0;
    int successful_tests = 0;
    
    Player player = player_AI2;
    printf("\nStarting full test with %d solutions...\n\n", solutionCount);
    
    clock_t start_time = clock();  // 记录开始时间
    
    // 测试所有solution
    for (int i = 0; i < solutionCount; i++) {
        strcpy(results[i].word, solutionList[i]);
        
        // 清除上一次的结果
        found_score = 0;
        found_guesses = 0;
        
        // 运行测试
        test_wordle(solutionList[i], &player);
        
        results[i].score = found_score;
        results[i].guesses = found_guesses;
        
        if (found_score > 0) {
            total_score += found_score;
            total_guesses += found_guesses;
            successful_tests++;
            printf("Test %d/%d: %s - Score: %d, Guesses: %d\n", 
                   i + 1, solutionCount, solutionList[i], found_score, found_guesses);
        } else {
            printf("Test %d/%d: %s - Failed\n", i + 1, solutionCount, solutionList[i]);
        }
        
        // 每100个测试输出一次统计
        if ((i + 1) % 100 == 0) {
            clock_t current_time = clock();
            double elapsed_time = ((double)(current_time - start_time)) / CLOCKS_PER_SEC;
            double avg_time_per_word = elapsed_time / (i + 1);
            double estimated_remaining_time = avg_time_per_word * (solutionCount - (i + 1));
            
            printf("\nIntermediate results after %d words:\n", i + 1);
            printf("Time elapsed: %.2f seconds\n", elapsed_time);
            printf("Average time per word: %.3f seconds\n", avg_time_per_word);
            printf("Estimated remaining time: %.2f seconds\n", estimated_remaining_time);
            printf("Successful tests: %d (%.2f%%)\n", 
                   successful_tests, (float)successful_tests / (i + 1) * 100);
            printf("Average score: %.2f\n", (float)total_score / successful_tests);
            printf("Average guesses: %.2f\n\n", (float)total_guesses / successful_tests);
        }
    }
    
    clock_t end_time = clock();
    double total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\nFinal Test Summary:\n");
    printf("Total time: %.2f seconds\n", total_time);
    printf("Average time per word: %.3f seconds\n", total_time / solutionCount);
    printf("Total words: %d\n", solutionCount);
    printf("Successful tests: %d\n", successful_tests);
    printf("Success rate: %.2f%%\n", (float)successful_tests / solutionCount * 100);
    printf("Average score: %.2f\n", (float)total_score / successful_tests);
    printf("Average guesses: %.2f\n", (float)total_guesses / successful_tests);
    
    // 输出猜测次数分布
    int guess_distribution[MAX_ROUNDS + 1] = {0};
    for (int i = 0; i < solutionCount; i++) {
        if (results[i].guesses > 0 && results[i].guesses <= MAX_ROUNDS) {
            guess_distribution[results[i].guesses]++;
        }
    }
    
    printf("\nGuess Distribution:\n");
    for (int i = 1; i <= MAX_ROUNDS; i++) {
        printf("%d guesses: %d words (%.2f%%)\n", 
               i, guess_distribution[i],
               (float)guess_distribution[i] / successful_tests * 100);
    }
    
    free(results);
    return 0;
}
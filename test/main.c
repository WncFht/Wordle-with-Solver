#include "wordle.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#define TEST_CASES 30

typedef struct {
    char word[WORD_LENGTH + 1];
    int score;
    int guesses;
} TestResult;

// 这个数组用来捕获每次调用 wordle 的最后一行输出（得分）
char last_output[256] = {0};
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

char* get_solution(bool* used, int* used_count) {
    if (*used_count >= SOLUTION_LENGTH) {
        return NULL;
    }
    
    int index;
    do {
        index = rand() % SOLUTION_LENGTH;
    } while (used[index]);
    
    used[index] = true;
    (*used_count)++;
    
    return solutionList[index];
}

int main() {
    srand(time(NULL));
    
    if (wordCount == 0) {
        loadWords("wordList.txt");
    }
    if (solutionCount == 0) {
        loadSolution("solutionList.txt");
    }
    
    bool* used = (bool*)calloc(SOLUTION_LENGTH, sizeof(bool));
    int used_count = 0;
    
    TestResult* results = (TestResult*)malloc(TEST_CASES * sizeof(TestResult));
    int total_score = 0;
    int total_guesses = 0;
    int successful_tests = 0;
    
    Player player = player_AI2;
    printf("\n");
    printf("Starting %d test cases...\n", TEST_CASES);
    
    for (int i = 0; i < TEST_CASES && i < SOLUTION_LENGTH; i++) {
        char* solution = get_solution(used, &used_count);
        if (!solution) {
            printf("Run out of solutions after %d tests\n", i);
            break;
        }
        
        strcpy(results[i].word, solution);
        
        // 清除上一次的结果
        found_score = 0;
        found_guesses = 0;
        
        // 运行测试
        test_wordle(solution, &player);
        
        results[i].score = found_score;
        results[i].guesses = found_guesses;
        
        if (found_score > 0) {
            total_score += found_score;
            total_guesses += found_guesses;
            successful_tests++;
            printf("Test %d/%d: %s - Score: %d, Guesses: %d\n", 
                   i + 1, TEST_CASES, solution, found_score, found_guesses);
        } else {
            printf("Test %d/%d: %s - Failed\n", i + 1, TEST_CASES, solution);
        }
        
        printf("\n");
        // 每100个测试输出一次中间结果
        if ((i + 1) % 100 == 0) {
            printf("\nIntermediate results after %d tests:\n", i + 1);
            printf("Successful tests so far: %d\n", successful_tests);
            printf("Current success rate: %.2f%%\n", 
                   (float)successful_tests / (i + 1) * 100);
            printf("Current average score: %.2f\n", 
                   (float)total_score / successful_tests);
            printf("Current average guesses: %.2f\n\n", 
                   (float)total_guesses / successful_tests);
        }
    }
    
    printf("\nFinal Test Summary:\n");
    printf("Total tests: %d\n", TEST_CASES);
    printf("Successful tests: %d\n", successful_tests);
    printf("Success rate: %.2f%%\n", 
           (float)successful_tests / TEST_CASES * 100);
    printf("Average score: %.2f\n", 
           (float)total_score / successful_tests);
    printf("Average guesses: %.2f\n", 
           (float)total_guesses / successful_tests);
    
    free(used);
    free(results);
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>  // Added for uint32_t

#define MAX_WORDS 15000
#define WORD_LEN 5
#define MAX_ATTEMPTS 10
#define PATTERN_SIZE 243
#define NUM_SOLUTIONS 2315
#define MAX_POSSIBLE MAX_WORDS

// Forward declarations
void getPattern(const char* guess, const char* target, char* pattern);
int patternToIndex(const char* pattern);
double calculateEntropyFast(int wordIdx);

typedef struct {
    int gamesPlayed;
    int gamesWon;
    double averageAttempts;
    int attemptDistribution[MAX_ATTEMPTS + 1];
    int totalScore;
    clock_t totalTime;
} Statistics;

typedef struct {
    uint32_t mask;     // Letter existence bitmask
    uint32_t pos[26];  // Position bitmask for each letter
} WordMask;

typedef struct {
    int word_index;
    double entropy;
    int possible_count;
} EntropyCache;


// 全局变量
WordMask wordMasks[MAX_WORDS];
int patterns[MAX_WORDS][MAX_WORDS];  // 预计算的模式
EntropyCache entropyCache[MAX_WORDS];
int cacheSize = 0;
int useOptimizedVersion = 0;  // 0 for normal, 1 for optimized
// 最优起始词序列
const char* BEST_OPENERS[] = {
    "CRANE", "SLATE", "TRACE", "SLANT", "PARSE"
};

// Global variables
char wordList[MAX_WORDS][WORD_LEN + 1];
char solutions[NUM_SOLUTIONS][WORD_LEN + 1];
char solution[WORD_LEN + 1];
int numWords = 0;
int numSolutions = 0;
int patternFreq[PATTERN_SIZE];
char* possibleSolutions[MAX_WORDS];  // 扩大数组大小
int numPossible = 0;
Statistics stats;

// Letter position weights
double letterWeights[26][WORD_LEN] = {0};


// 预计算所有单词的掩码
void precomputeWordMasks() {
    for(int i = 0; i < numWords; i++) {
        wordMasks[i].mask = 0;
        memset(wordMasks[i].pos, 0, sizeof(wordMasks[i].pos));
        
        for(int j = 0; j < WORD_LEN; j++) {
            int letter = wordList[i][j] - 'A';
            wordMasks[i].mask |= (1 << letter);
            wordMasks[i].pos[letter] |= (1 << j);
        }
    }
}

// 预计算所有模式
void precomputePatterns() {
    for(int i = 0; i < numWords; i++) {
        for(int j = 0; j < numWords; j++) {
            char pattern[WORD_LEN + 1];
            getPattern(wordList[i], wordList[j], pattern);
            patterns[i][j] = patternToIndex(pattern);
        }
    }
}

// 优化的getPattern实现
int getPatternFast(int guessIdx, int targetIdx) {
    return patterns[guessIdx][targetIdx];
}

// 使用缓存的熵计算
double getCachedEntropy(const char* word) {
    int wordIdx = -1;
    // 找到word在wordList中的索引
    for(int i = 0; i < numWords; i++) {
        if(strcmp(word, wordList[i]) == 0) {
            wordIdx = i;
            break;
        }
    }

    // 检查缓存
    for(int i = 0; i < cacheSize; i++) {
        if(entropyCache[i].word_index == wordIdx && 
           entropyCache[i].possible_count == numPossible) {
            return entropyCache[i].entropy;
        }
    }

    // 计算新的熵值
    double entropy = calculateEntropyFast(wordIdx);

    // 更新缓存
    if(cacheSize < MAX_WORDS) {
        entropyCache[cacheSize].word_index = wordIdx;
        entropyCache[cacheSize].entropy = entropy;
        entropyCache[cacheSize].possible_count = numPossible;
        cacheSize++;
    }

    return entropy;
}

// 优化的熵计算
double calculateEntropyFast(int wordIdx) {
    memset(patternFreq, 0, sizeof(patternFreq));
    
    for(int i = 0; i < numPossible; i++) {
        int targetIdx = -1;
        for(int j = 0; j < numWords; j++) {
            if(wordList[j] == possibleSolutions[i]) {
                targetIdx = j;
                break;
            }
        }
        if(targetIdx != -1) {
            patternFreq[getPatternFast(wordIdx, targetIdx)]++;
        }
    }
    
    double entropy = 0.0;
    for(int i = 0; i < PATTERN_SIZE; i++) {
        if(patternFreq[i] > 0) {
            double prob = (double)patternFreq[i] / numPossible;
            entropy -= prob * log2(prob);
        }
    }
    
    return entropy;
}

// 优化的评分函数
double scoreWordFast(const char* word, int remainingAttempts) {
    double score = getCachedEntropy(word);
    
    if(numPossible > 10) {
        double weightScore = 0;
        for(int i = 0; i < WORD_LEN; i++) {
            weightScore += letterWeights[word[i] - 'A'][i];
        }
        score += weightScore * 0.1;
    }
    
    if(numPossible <= 10) {
        for(int i = 0; i < numPossible; i++) {
            if(strcmp(word, possibleSolutions[i]) == 0) {
                score += 0.2;
                break;
            }
        }
    }
    
    return score;
}

// 优化的最佳猜测函数
char* findBestGuessFast(int remainingAttempts) {
    static char bestGuess[WORD_LEN + 1];
    double bestScore = -1;
    
    // 使用最优起始序列
    if(numPossible == numSolutions) {
        for(int i = 0; i < sizeof(BEST_OPENERS)/sizeof(BEST_OPENERS[0]); i++) {
            double score = scoreWordFast(BEST_OPENERS[i], remainingAttempts);
            if(score > bestScore) {
                bestScore = score;
                strcpy(bestGuess, BEST_OPENERS[i]);
            }
        }
        if(bestScore > -1) {
            return bestGuess;
        }
        return "CRANE";  // 默认起始词
    }
    
    if(numPossible == 2) return possibleSolutions[0];
    
    // 动态搜索范围
    int searchLimit = (numPossible > 1000) ? numSolutions : numWords;
    for(int i = 0; i < searchLimit; i++) {
        double score = scoreWordFast(wordList[i], remainingAttempts);
        if(score > bestScore) {
            bestScore = score;
            strcpy(bestGuess, wordList[i]);
        }
    }
    
    printf("Cache size: %d\n", cacheSize);
    return bestGuess;
}

void loadWords() {
    FILE* fp = fopen("wordList.txt", "r");
    if (!fp) {
        printf("Error: Cannot open wordList.txt\n");
        exit(1);
    }
    
    while (fscanf(fp, "%s", wordList[numWords]) != EOF && numWords < MAX_WORDS) {
        for (int i = 0; wordList[numWords][i]; i++) {
            wordList[numWords][i] = toupper(wordList[numWords][i]);
        }
        numWords++;
    }
    fclose(fp);

    fp = fopen("solutions.txt", "r");
    if (!fp) {
        printf("Warning: solutions.txt not found, using first %d words from wordList.txt\n", NUM_SOLUTIONS);
        memcpy(solutions, wordList, sizeof(char) * NUM_SOLUTIONS * (WORD_LEN + 1));
        numSolutions = NUM_SOLUTIONS;
    } else {
        while (fscanf(fp, "%s", solutions[numSolutions]) != EOF && numSolutions < NUM_SOLUTIONS) {
            for (int i = 0; solutions[numSolutions][i]; i++) {
                solutions[numSolutions][i] = toupper(solutions[numSolutions][i]);
            }
            numSolutions++;
        }
        fclose(fp);
    }
    printf("Loaded %d total words and %d solutions\n", numWords, numSolutions);
}

void calculateLetterWeights() {
    memset(letterWeights, 0, sizeof(letterWeights));
    for (int i = 0; i < numSolutions; i++) {
        for (int j = 0; j < WORD_LEN; j++) {
            letterWeights[solutions[i][j] - 'A'][j] += 1.0 / numSolutions;
        }
    }
}

int patternToIndex(const char* pattern) {
    int index = 0;
    for(int i = 0; i < WORD_LEN; i++) {
        index = index * 3 + (pattern[i] == 'G' ? 2 : pattern[i] == 'Y' ? 1 : 0);
    }
    return index;
}

void getPattern(const char* guess, const char* target, char* pattern) {
    int letterCount[26] = {0};
    int used[WORD_LEN] = {0};
    memset(pattern, 'B', WORD_LEN);
    pattern[WORD_LEN] = '\0';
    
    // Count letters in target
    for(int i = 0; i < WORD_LEN; i++) {
        letterCount[target[i] - 'A']++;
    }
    
    // Mark greens first
    for(int i = 0; i < WORD_LEN; i++) {
        if(guess[i] == target[i]) {
            pattern[i] = 'G';
            used[i] = 1;
            letterCount[guess[i] - 'A']--;
        }
    }
    
    // Then mark yellows
    for(int i = 0; i < WORD_LEN; i++) {
        if(!used[i] && letterCount[guess[i] - 'A'] > 0) {
            pattern[i] = 'Y';
            letterCount[guess[i] - 'A']--;
        }
    }
}

double calculateEntropy(const char* word) {
    memset(patternFreq, 0, sizeof(patternFreq));
    char pattern[WORD_LEN + 1];
    
    for(int i = 0; i < numPossible; i++) {
        getPattern(word, possibleSolutions[i], pattern);
        patternFreq[patternToIndex(pattern)]++;
    }
    
    double entropy = 0.0;
    for(int i = 0; i < PATTERN_SIZE; i++) {
        if(patternFreq[i] > 0) {
            double prob = (double)patternFreq[i] / numPossible;
            entropy -= prob * log2(prob);
        }
    }
    
    return entropy;
}

double scoreWord(const char* word, int remainingAttempts) {
    double score = calculateEntropy(word);
    
    if(numPossible > 10) {
        double weightScore = 0;
        for(int i = 0; i < WORD_LEN; i++) {
            weightScore += letterWeights[word[i] - 'A'][i];
        }
        score += weightScore * 0.1;
    }
    
    if(numPossible <= 10) {
        for(int i = 0; i < numPossible; i++) {
            if(strcmp(word, possibleSolutions[i]) == 0) {
                score += 0.2;
                break;
            }
        }
    }
    
    if(remainingAttempts <= 2 && numPossible <= 50) {
        for(int i = 0; i < numPossible; i++) {
            if(strcmp(word, possibleSolutions[i]) == 0) {
                score *= 2.0;
                break;
            }
        }
    }
    
    return score;
}

char* findBestGuess(int remainingAttempts) {
    static char bestGuess[WORD_LEN + 1];
    double bestScore = -1;
    
    if(numPossible == numSolutions) return "CRANE";
    if(numPossible == 2) return possibleSolutions[0];
    
    if(numPossible > 10) {
        for(int i = 0; i < numWords; i++) {
            double score = scoreWord(wordList[i], remainingAttempts);
            if(score > bestScore) {
                bestScore = score;
                strcpy(bestGuess, wordList[i]);
            }
        }
    } else {
        for(int i = 0; i < numPossible; i++) {
            double score = scoreWord(possibleSolutions[i], remainingAttempts);
            if(score > bestScore) {
                bestScore = score;
                strcpy(bestGuess, possibleSolutions[i]);
            }
        }
    }
    
    return bestGuess;
}

void updatePossibleSolutions(const char* guess, const char* result) {
    int newCount = 0;
    char pattern[WORD_LEN + 1];
    char* newPossible[NUM_SOLUTIONS];
    
    for(int i = 0; i < numPossible; i++) {
        getPattern(guess, possibleSolutions[i], pattern);
        if(strcmp(pattern, result) == 0) {
            newPossible[newCount] = possibleSolutions[i];
            newCount++;
        }
    }
    
    for(int i = 0; i < newCount; i++) {
        possibleSolutions[i] = newPossible[i];
    }
    
    numPossible = newCount;
}

void initGame(int gameMode) {
    switch(gameMode) {
        case 1: // Random words from solutions
        case 4: // Standard Wordle test
        case 3: // Single word test
            numPossible = numSolutions;
            for(int i = 0; i < numSolutions; i++) {
                possibleSolutions[i] = solutions[i];
            }
            break;
            
        case 2: // All words test - use full wordlist
            numPossible = numWords;
            for(int i = 0; i < numWords; i++) {
                possibleSolutions[i] = wordList[i];
            }
            break;
    }
}

int calculateScore(int attempts) {
    if (attempts > MAX_ATTEMPTS) return 0;
    return 11 - attempts;
}

void resetStats() {
    memset(&stats, 0, sizeof(stats));
    cacheSize = 0;
}

void updateStats(int attempts) {
    stats.gamesPlayed++;
    if (attempts <= MAX_ATTEMPTS) {
        stats.gamesWon++;
        stats.attemptDistribution[attempts-1]++;
        stats.averageAttempts = (stats.averageAttempts * (stats.gamesWon-1) + attempts) / stats.gamesWon;
        stats.totalScore += calculateScore(attempts);
    }
}

int playGame(int verbose, int gameMode) {
    char result[WORD_LEN + 1];
    char* guess;
    int attempts = 0;
    clock_t gameStart = clock();
    
    initGame(gameMode);
    
    while(attempts < MAX_ATTEMPTS) {
        // 根据选择使用不同的函数
        guess = useOptimizedVersion ? 
            findBestGuessFast(MAX_ATTEMPTS - attempts) : 
            findBestGuess(MAX_ATTEMPTS - attempts);
        
        if(verbose) {
            printf("Guess %d: %s\n", attempts + 1, guess);
            printf("Possible solutions remaining: %d\n", numPossible);
            if(numPossible <= 10) {
                printf("Remaining words: ");
                for(int i = 0; i < numPossible; i++) {
                    printf("%s ", possibleSolutions[i]);
                }
                printf("\n");
            }
        }
        
        getPattern(guess, solution, result);
        if(verbose) printf("Result: %s\n", result);
        
        if(strcmp(result, "GGGGG") == 0) {
            stats.totalTime += clock() - gameStart;
            return attempts + 1;
        }
        
        updatePossibleSolutions(guess, result);
        attempts++;
    }
    
    stats.totalTime += clock() - gameStart;
    return MAX_ATTEMPTS + 1;
}

void printStats() {
    printf("\n=== Final Statistics ===\n");
    printf("Games Played: %d\n", stats.gamesPlayed);
    printf("Win Rate: %.2f%%\n", (double)stats.gamesWon/stats.gamesPlayed*100);
    printf("Average Attempts: %.2f\n", stats.averageAttempts);
    printf("Total Score: %d\n", stats.totalScore);
    printf("Average Score per Game: %.2f\n", (double)stats.totalScore/stats.gamesPlayed);
    printf("\nAttempt Distribution:\n");
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        printf("%d attempts: %d (%.2f%%) - %d points each\n", 
               i+1, 
               stats.attemptDistribution[i],
               (double)stats.attemptDistribution[i]/stats.gamesPlayed*100,
               calculateScore(i+1));
    }
    printf("Failed: %d (%.2f%%) - 0 points\n", 
           stats.gamesPlayed - stats.gamesWon,
           (double)(stats.gamesPlayed - stats.gamesWon)/stats.gamesPlayed*100);
    printf("Average Time: %.3f seconds\n", 
           (double)stats.totalTime/CLOCKS_PER_SEC/stats.gamesPlayed);
    printf("\nAlgorithm version: %s\n", 
            useOptimizedVersion ? "Optimized" : "Normal");
}

// 修改main函数
int main() {
    loadWords();
    calculateLetterWeights();
    precomputeWordMasks();
    precomputePatterns();
    srand(time(NULL));
    
    char continueRunning = 'y';
    while(continueRunning == 'y' || continueRunning == 'Y') {
        resetStats();  // 重置统计数据
        
        printf("\nUse optimized version? (0 for normal, 1 for optimized): ");
        scanf("%d", &useOptimizedVersion);
        
        int testMode;
        printf("\nSelect test mode:\n");
        printf("1. Test specific number of random words from solution set\n");
        printf("2. Test all valid words\n");
        printf("3. Test single word\n");
        printf("4. Test all standard Wordle solutions (2315 words)\n");
        scanf("%d", &testMode);
        
        clock_t startTime = clock();
    
        switch(testMode) {
            case 1: {
                int numTests;
                printf("Enter number of words to test: ");
                scanf("%d", &numTests);
                numTests = (numTests > numSolutions) ? numSolutions : numTests;
                
                for (int i = 0; i < numTests; i++) {
                    int wordIdx = rand() % numSolutions;
                    strcpy(solution, solutions[wordIdx]);
                    int attempts = playGame(0, testMode);
                    updateStats(attempts);
                    
                    if ((i+1) % 100 == 0)
                        printf("Completed %d words...\n", i+1);
                }
                break;
            }
            case 2: {
                printf("Testing all %d valid words...\n", numWords);
                for (int i = 0; i < numWords; i++) {
                    strcpy(solution, wordList[i]);
                    int attempts = playGame(0, testMode);
                    updateStats(attempts);
                    printf(solution);
                    printf("Score for this word: %d\n", calculateScore(attempts));
                    if ((i+1) % 100 == 0)
                        printf("Completed %d words...\n", i+1);
                }
                break;
            }
            case 3: {
                char testWord[WORD_LEN + 1];
                printf("Enter word to test: ");
                scanf("%s", testWord);
                for (int i = 0; testWord[i]; i++)
                    testWord[i] = toupper(testWord[i]);
                    
                // Validate input word
                int valid = 0;
                for (int i = 0; i < numWords; i++) {
                    if (strcmp(testWord, wordList[i]) == 0) {
                        valid = 1;
                        break;
                    }
                }
                
                if (!valid) {
                    printf("Invalid word: Not in word list\n");
                    return 1;
                }
                
                strcpy(solution, testWord);
                int attempts = playGame(1, testMode);
                updateStats(attempts);
                printf("Score for this word: %d\n", calculateScore(attempts));
                break;
            }
            case 4: {
                printf("Testing all %d standard Wordle solutions...\n", numSolutions);
                printf("This may take a few minutes...\n");
                
                for (int i = 0; i < numSolutions; i++) {
                    strcpy(solution, solutions[i]);
                    int attempts = playGame(0, testMode);
                    updateStats(attempts);
                    // printf("Score for this word: %d\n", calculateScore(attempts));
                    if ((i+1) % 100 == 0) {
                            printf("Completed %d words... ", i+1);
                            printf("Current Average: %.2f\n", stats.averageAttempts);
                        }
                }
                break;
            }
            default: {
                printf("Invalid test mode selected\n");
                return 1;
            }
        }
    
        stats.totalTime = clock() - startTime;
        printStats();

        // Additional statistics for standard mode
        if (testMode == 4) {
            printf("\nDetailed Performance Analysis:\n");
            printf("Best possible score: %d\n", stats.gamesPlayed * 10);
            printf("Actual score: %d\n", stats.totalScore);
            printf("Efficiency: %.2f%%\n", 
                (double)stats.totalScore/(stats.gamesPlayed * 10) * 100);
            
            int score_distribution[11] = {0};  // 0-10 points
            for (int i = 0; i < MAX_ATTEMPTS; i++) {
                score_distribution[calculateScore(i+1)] += stats.attemptDistribution[i];
            }
            score_distribution[0] = stats.gamesPlayed - stats.gamesWon;
            
            printf("\nScore Distribution:\n");
            for (int i = 10; i >= 0; i--) {
                if (score_distribution[i] > 0) {
                    printf("%2d points: %4d words (%.2f%%)\n", 
                        i, 
                        score_distribution[i],
                        (double)score_distribution[i]/stats.gamesPlayed*100);
                }
            }
        }
    
        printf("\nDo you want to continue testing? (y/n): ");
        while(getchar() != '\n');  // 清除输入缓冲
        scanf("%c", &continueRunning);
    }
    
    printf("Thank you for using the Wordle solver!\n");
    return 0;
}
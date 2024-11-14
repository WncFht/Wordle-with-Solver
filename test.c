#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 150000  // 假设 wordList.txt 中最多有 12000 个单词
#define WORD_LENGTH 5    // 单词长度为 5
#define MAX_GUESSES 6    // 最多猜测次数为 6

char allWords[MAX_WORDS][WORD_LENGTH + 1];     // 存储所有合法单词
char solutionWords[2315][WORD_LENGTH + 1];     // 存储可能的解决方案单词
int totalWords = 0;       // 总的合法单词数量
int totalSolutions = 0;   // 总的解决方案单词数量

// 读取 wordList.txt 文件，填充 allWords 和 solutionWords 数组
void readWordList() {
    FILE *file = fopen("wordList.txt", "r");
    if (file == NULL) {
        printf("无法打开 wordList.txt 文件。\n");
        exit(1);
    }

    char word[WORD_LENGTH + 2];  // 包含换行符和字符串结束符
    while (fgets(word, sizeof(word), file)) {
        // 移除换行符
        word[strcspn(word, "\n")] = '\0';
        if (strlen(word) == WORD_LENGTH) {
            strcpy(allWords[totalWords], word);
            totalWords++;
        }
    }

    fclose(file);

    // 选择前 2315 个单词作为解决方案单词（这里假设前 2315 个是常用单词）
    if (totalWords < 2315) {
        printf("单词数量不足 2315 个。\n");
        exit(1);
    }

    for (int i = 0; i < 2315; i++) {
        strcpy(solutionWords[i], allWords[i]);
        totalSolutions++;
    }
}

// 检查用户猜测的单词是否在合法单词列表中
int isValidWord(char *guess) {
    for (int i = 0; i < totalWords; i++) {
        if (strcmp(guess, allWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// 生成反馈结果
void generateFeedback(char *guess, char *answer, char *feedback) {
    int usedInAnswer[WORD_LENGTH] = {0};  // 标记答案中已匹配的字母

    // 初始化反馈结果
    for (int i = 0; i < WORD_LENGTH; i++) {
        feedback[i] = 'B';
    }
    feedback[WORD_LENGTH] = '\0';

    // 首先检查位置和字母都匹配的情况
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == answer[i]) {
            feedback[i] = 'G';
            usedInAnswer[i] = 1;
        }
    }

    // 然后检查字母匹配但位置不对的情况
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == 'G') {
            continue;  // 已经匹配过
        }
        for (int j = 0; j < WORD_LENGTH; j++) {
            if (!usedInAnswer[j] && guess[i] == answer[j]) {
                feedback[i] = 'Y';
                usedInAnswer[j] = 1;
                break;
            }
        }
    }
}

int main() {
    readWordList();

    // 初始化随机数种子
    srand((unsigned int)time(NULL));

    // 随机选择一个解决方案单词
    char answer[WORD_LENGTH + 1];
    strcpy(answer, solutionWords[rand() % totalSolutions]);

    // printf("调试用答案：%s\n", answer);  // 如果需要调试，可以打印答案

    char guess[WORD_LENGTH + 2];  // 包含字符串结束符
    int attempts = 0;
    int guessedCorrectly = 0;

    printf("欢迎来到 Wordle 游戏！您有 6 次机会来猜测一个 5 个字母的单词。\n");

    while (attempts < MAX_GUESSES) {
        printf("请输入您的猜测（第 %d 次）：", attempts + 1);
        fgets(guess, sizeof(guess), stdin);

        // 移除换行符
        guess[strcspn(guess, "\n")] = '\0';

        // 转换为小写字母
        for (int i = 0; i < strlen(guess); i++) {
            if (guess[i] >= 'A' && guess[i] <= 'Z') {
                guess[i] = guess[i] - 'A' + 'a';
            }
        }

        if (strlen(guess) != WORD_LENGTH) {
            printf("请输入一个 5 个字母的单词。\n");
            continue;
        }

        if (!isValidWord(guess)) {
            printf("该单词不在合法单词列表中，请重新输入。\n");
            continue;
        }

        attempts++;

        char feedback[WORD_LENGTH + 1];
        generateFeedback(guess, answer, feedback);

        printf("反馈结果：%s\n", feedback);

        if (strcmp(guess, answer) == 0) {
            guessedCorrectly = 1;
            break;
        }
    }

    if (guessedCorrectly) {
        printf("恭喜您，猜对了！\n");
    } else {
        printf("很遗憾，您没有猜中。正确答案是：%s\n", answer);
    }

    return 0;
}

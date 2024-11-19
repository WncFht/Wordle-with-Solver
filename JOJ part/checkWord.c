#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 检查单词并返回反馈结果
void check_word(const char* answer, const char* guess, char* feedback) {
    // 用于标记已使用的字母位置
    int used[5] = {0}; 
    
    // 转换答案为大写，便于比较
    char ans[6];
    strcpy(ans, answer);
    for(int i = 0; ans[i]; i++) {
        ans[i] = toupper(ans[i]);
    }
    
    // 转换猜测为大写
    char gus[6];
    strcpy(gus, guess);
    for(int i = 0; gus[i]; i++) {
        gus[i] = toupper(gus[i]);
    }
    
    // 第一遍：标记完全匹配的字母(G)
    for(int i = 0; i < 5; i++) {
        if(gus[i] == ans[i]) {
            feedback[i] = 'G';
            used[i] = 1;
        } else {
            feedback[i] = 'B';
        }
    }
    
    // 第二遍：标记位置不正确的字母(Y)
    for(int i = 0; i < 5; i++) {
        if(feedback[i] == 'B') {  // 如果当前位置还未标记为G
            for(int j = 0; j < 5; j++) {
                // 如果在其他未使用的位置找到相同字母
                if(!used[j] && gus[i] == ans[j]) {
                    feedback[i] = 'Y';
                    used[j] = 1;
                    break;
                }
            }
        }
    }
    
    // 添加字符串结束符
    feedback[5] = '\0';
}

// 如果需要独立测试，可以加入main函数

int main() {
    char answer[6], guess[6], feedback[6];

    // 提示用户输入两个单词
    scanf("%5s %5s", answer, guess); // 读取两个单词，最多5个字符

    // 调用函数检查单词
    check_word(answer, guess, feedback);

    // 输出结果
    printf("%s\n", feedback); // 输出结果

    return 0;
}
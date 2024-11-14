// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "wordle.h"
#include "wordle_solver.h"

// 颜色代码
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define GRAY "\033[90m"

// 状态显示相关
typedef struct {
    int games_played;
    int games_won;
    int current_streak;
    int max_streak;
    int guess_distribution[MAX_GUESSES + 1];
} GameStats;

// 全局统计数据
static GameStats stats = {0};

// 函数声明
void print_instructions();
void print_colored_feedback(const char* word, const char* feedback);
void print_keyboard_state(char* all_guesses[], char* all_feedback[], int guess_count);
void update_stats(int won, int guesses);
void print_stats();
void save_stats();
void load_stats();
void clear_screen();

// 主函数

int main(int argc, char* argv[]) {
    WordleGame game;
    char feedback[WORD_LENGTH + 1];
    char last_feedback[WORD_LENGTH + 1] = "BBBBB";
    char* all_guesses[MAX_GUESSES];
    char* all_feedback[MAX_GUESSES];
    int guess_count = 0;
    int auto_mode = 1;    // 添加这行
    int debug_mode = 0;   // 添加这行
    
    // 处理命令行参数
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--auto") == 0) auto_mode = 1;
        if(strcmp(argv[i], "--debug") == 0) debug_mode = 1;
    }

    // 初始化随机数生成器
    srand(time(NULL));
    
    // 载入统计数据
    load_stats();

    // 初始化游戏
    if(wordle_init(&game, "wordList.txt") != WORDLE_SUCCESS) {
        printf("Error: Failed to initialize game.\n");
        return 1;
    }

    // 初始化求解器
    init_solver("wordList.txt");

    // 分配内存
    for(int i = 0; i < MAX_GUESSES; i++) {
        all_guesses[i] = (char*)malloc(WORD_LENGTH + 1);
        all_feedback[i] = (char*)malloc(WORD_LENGTH + 1);
    }

    // 设置随机解决方案
    wordle_set_solution(&game, NULL);

    if(debug_mode) {
        printf("Solution: %s\n", game.currentSolution);
    }

    clear_screen();
    print_instructions();

    // 游戏主循环
    int won = 0;
    while(game.remainingGuesses > 0) {
        char* guess;
        
        printf("\nRound %d (%d guesses remaining)\n", 
               MAX_GUESSES - game.remainingGuesses + 1, 
               game.remainingGuesses);

        // 显示键盘状态
        print_keyboard_state(all_guesses, all_feedback, guess_count);

        // 获取猜测 (自动模式或手动输入)
        if(auto_mode) {
            guess = player_optimal(last_feedback);
            printf("AI guesses: %s\n", guess);
        } else {
            static char input[100];
            printf("Enter your guess: ");
            if(scanf("%s", input) != 1) {
                printf("Error reading input. Please try again.\n");
                continue;
            }
            
            // 转换为大写
            for(int i = 0; input[i]; i++) {
                input[i] = toupper(input[i]);
            }
            guess = input;
        }

        // 验证猜测
        WordleStatus status = wordle_make_guess(&game, guess, feedback);
        
        switch(status) {
            case WORDLE_SUCCESS:
                // 保存这次猜测
                strcpy(all_guesses[guess_count], guess);
                strcpy(all_feedback[guess_count], feedback);
                strcpy(last_feedback, feedback);
                guess_count++;
                
                // 显示结果
                clear_screen();
                printf("\nGame Progress:\n");
                for(int i = 0; i < guess_count; i++) {
                    print_colored_feedback(all_guesses[i], all_feedback[i]);
                }
                
                // 检查是否获胜
                if(strcmp(feedback, "GGGGG") == 0) {
                    printf("\nCongratulations! You've won in %d attempts!\n", 
                           MAX_GUESSES - game.remainingGuesses + 1);
                    won = 1;
                    break;
                }
                continue;
                
            case WORDLE_INVALID_WORD:
                printf("Word not in list. Please try again.\n");
                continue;
                
            case WORDLE_INVALID_INPUT:
                printf("Invalid input. Please enter a %d-letter word.\n", WORD_LENGTH);
                continue;
                
            default:
                printf("An error occurred. Please try again.\n");
                continue;
        }
        
        if(won) break;
    }

    // 游戏结束
    if(!won) {
        printf("\nGame Over! The word was: %s\n", game.currentSolution);
    }

    // 更新统计数据
    update_stats(won, MAX_GUESSES - game.remainingGuesses);
    print_stats();
    save_stats();

    // 清理资源
    for(int i = 0; i < MAX_GUESSES; i++) {
        free(all_guesses[i]);
        free(all_feedback[i]);
    }
    wordle_cleanup(&game);

    return 0;
}

// 清屏函数
void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// 打印游戏说明
void print_instructions() {
    printf(BOLD"\n=== Wordle ===\n"RESET);
    printf("Try to guess the %d-letter word. You have %d attempts.\n", 
           WORD_LENGTH, MAX_GUESSES);
    printf("\nAfter each guess, you'll get feedback:\n");
    printf(GREEN"G"RESET" = Correct letter in correct position\n");
    printf(YELLOW"Y"RESET" = Correct letter in wrong position\n");
    printf(GRAY"B"RESET" = Letter not in word\n\n");
}

// 显示带颜色的反馈
void print_colored_feedback(const char* word, const char* feedback) {
    for(int i = 0; i < WORD_LENGTH; i++) {
        switch(feedback[i]) {
            case 'G':
                printf(GREEN"%c"RESET, word[i]);
                break;
            case 'Y':
                printf(YELLOW"%c"RESET, word[i]);
                break;
            case 'B':
                printf(GRAY"%c"RESET, word[i]);
                break;
            default:
                printf("%c", word[i]);
        }
    }
    printf("\n");
}

// 显示键盘状态
    void print_keyboard_state(char* all_guesses[], char* all_feedback[], int guess_count) {
        const char* keyboard = "QWERTYUIOP\n ASDFGHJKL\n  ZXCVBNM";
        char key_status[26] = {0}; // G, Y, B, or 0 for unused
        
        // 根据之前的猜测更新键盘状态
        for(int i = 0; i < guess_count; i++) {
            for(int j = 0; j < WORD_LENGTH; j++) {
                char c = all_guesses[i][j];
                char current = all_feedback[i][j];
                int idx = c - 'A';
                
                // 只在状态更好时更新
                if(key_status[idx] == 0 || 
                   (key_status[idx] == 'B' && (current == 'Y' || current == 'G')) ||
                   (key_status[idx] == 'Y' && current == 'G')) {
                    key_status[idx] = current;
                }
            }
        }
        
        // 打印键盘
        printf("\nKeyboard:\n");
        for(int i = 0; keyboard[i]; i++) {
            if(keyboard[i] == '\n') {
                printf("\n");
                continue;
            }
            if(keyboard[i] == ' ') {
                printf(" ");
                continue;
            }
            
            char c = keyboard[i];
            char status = key_status[c - 'A'];
            
            switch(status) {
                case 'G':
                    printf(GREEN"%c"RESET, c);
                    break;
                case 'Y':
                    printf(YELLOW"%c"RESET, c);
                    break;
                case 'B':
                    printf(GRAY"%c"RESET, c);
                    break;
                default:
                    printf("%c", c);
            }
        }
        printf("\n");
    }

// 更新游戏统计
void update_stats(int won, int guesses) {
    stats.games_played++;
    if(won) {
        stats.games_won++;
        stats.current_streak++;
        if(stats.current_streak > stats.max_streak) {
            stats.max_streak = stats.current_streak;
        }
        stats.guess_distribution[guesses]++;
    } else {
        stats.current_streak = 0;
    }
}

// 打印统计数据
void print_stats() {
    printf("\nGame Statistics:\n");
    printf("Games Played: %d\n", stats.games_played);
    printf("Win Rate: %.1f%%\n", 
           stats.games_played > 0 ? (float)stats.games_won / stats.games_played * 100 : 0);
    printf("Current Streak: %d\n", stats.current_streak);
    printf("Max Streak: %d\n", stats.max_streak);
    
    printf("\nGuess Distribution:\n");
    int max_count = 0;
    for(int i = 1; i <= MAX_GUESSES; i++) {
        if(stats.guess_distribution[i] > max_count) {
            max_count = stats.guess_distribution[i];
        }
    }
    
    for(int i = 1; i <= MAX_GUESSES; i++) {
        printf("%d: ", i);
        int bars = max_count > 0 ? 
                  (int)((float)stats.guess_distribution[i] / max_count * 20) : 0;
        for(int j = 0; j < bars; j++) printf("█");
        printf(" %d\n", stats.guess_distribution[i]);
    }
}

// 保存统计数据
void save_stats() {
    FILE* fp = fopen("wordle_stats.dat", "wb");
    if(fp) {
        fwrite(&stats, sizeof(GameStats), 1, fp);
        fclose(fp);
    }
}

// 载入统计数据
void load_stats() {
    FILE* fp = fopen("wordle_stats.dat", "rb");
    if(fp) {
        fread(&stats, sizeof(GameStats), 1, fp);
        fclose(fp);
    }
}
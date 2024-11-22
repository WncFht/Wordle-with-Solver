
# Wordle Game

The Wordle Game is a console-based implementation of the popular word-guessing game. Players need to guess a 5-letter word within six attempts, receiving feedback after each guess to refine their guesses.

---

## Overview of Program Implementation

 Below is an explanation of how the program is structured and how the various components interact to realize its functionality.

---

### 1. **Core Structure**
The program consists of several key components:
- **`main.c`**: The entry point of the program. It initializes the game, loads the word list, and sets up the player mode.
- **`wordle.c`**: Handles the core game mechanics, including loading the word list, validating guesses, generating feedback, and managing game rounds.
- **`players.c`**: Implements different player strategies (e.g., human input, random guessing, and AI-based approaches like entropy and minimax).
- **`wordle.h`**: Contains constant definitions (e.g., word length, maximum attempts) and function declarations.

---

### 2. **Program Flow**

#### a. **Initialization**
- **Load Word List**:
  - Reads a file (`wordList.txt`) containing valid 5-letter words.
  - Stores the words in a global array for lookup and selection during the game.

- **Random Word Selection**:
  - Chooses a random word from the loaded word list to be the solution for the game.

#### b. **Gameplay**
- The game is played over multiple rounds (up to 6 attempts by default).
- Each round involves:
  1. The player (human or AI) making a guess.
  2. Generating feedback for the guess based on how it compares to the solution:
     - `G` (Green): Correct letter and correct position.
     - `Y` (Yellow): Correct letter, wrong position.
     - `B` (Black): Letter not present in the solution.
  3. Providing feedback to the player for subsequent guesses.
  4. Checking if the solution has been guessed correctly or if attempts are exhausted.

#### c. **Player Modes**
- **Human Input**:
  - Reads guesses from the console, validates them against the word list, and ensures correct formatting (uppercase, valid length, etc.).
- **Random Guessing**:
  - Selects random words from the dictionary for each guess.
- **AI Strategies**:
  - **Entropy-Based**:
    - Calculates the entropy of potential guesses to maximize information gained.
  - **Minimax**:
    - Attempts to minimize the maximum remaining solution set in the worst case.
  - **Frequency-Based**:
    - Scores guesses based on letter frequency to increase the likelihood of correct guesses.

---

### 3. **Key Functions**

#### a. `load_word_list` (in `wordle.c`)
- Reads the `wordList.txt` file and loads words into memory.
- Validates word length and ensures all words are uppercase.

#### b. `generate_feedback` (in `wordle.c`)
- Compares the guessed word to the solution.
- Assigns feedback (`G`, `Y`, `B`) for each letter based on its position and presence in the solution.

#### c. `wordle` (in `wordle.c`)
- Main game logic that controls rounds, feedback generation, and player interactions.
- Invokes the player's guess function and determines if the game is won or lost.

#### d. AI Functions (in `players.c`)
- `player_entropy`: Maximizes information gain by calculating the entropy of potential guesses.
- `player_minimax`: Reduces the solution space by minimizing the worst-case scenario.
- `player_frequency`: Prioritizes guesses based on letter frequency.

---

### 4. **Customization**
- **Change Game Settings**: Modify constants in `wordle.h`:
  - `WORD_LENGTH`: Change the length of the word (e.g., 4-letter or 6-letter Wordle).
  - `MAX_ROUNDS`: Adjust the number of allowed attempts.
- **Expand the Word List**: Add more words to `wordList.txt` to make the game more challenging.
- **AI Improvements**: Experiment with AI strategies to make guessing more efficient.

---

### 5. **Program Flow Diagram**

1. **Game Initialization**:
   - Load `wordList.txt`.
   - Randomly select a solution word.

2. **Game Loop (6 Rounds)**:
   - Player makes a guess (human or AI).
   - Feedback is generated for the guess.
   - Player adjusts the next guess based on feedback.
   - Check if the player has guessed the solution correctly.

3. **End of Game**:
   - Display success message if the solution is guessed.
   - Reveal the solution if the player runs out of attempts.


---

## Installation and Build

### Prerequisites

- CMake (version 3.10 or higher).
- A C Compiler:
  - Windows: MSVC (Visual Studio) or GCC (MinGW).
  - Ubuntu: GCC or Clang.
- `wordList.txt` containing valid 5-letter words (one word per line).

---

### Build Instructions

#### Windows (Visual Studio)

1. **Generate Build Files**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

2. **Build the Project**:
   Use Visual Studio to open the generated `WordleGame.sln` file, or build from the command line:
   ```bash
   cmake --build .
   ```

3. **Run the Game**:
   The executable will be available in the `build/Debug/` directory:
   ```bash
   cd Debug
   wordle.exe
   ```

**Remember to move the wordlists.txt to the right position.**
---

#### Just in Console

```
gcc -o wordle ./main.c ./players.c ./wordle.c ./wordle.h
./wordle
```

And you can modify the code in main to test different players.

```
// Start game with different players.
// TEST HERE!!!
wordle(solution, player_input);
```

#### Ubuntu

1. **Install Required Tools**:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake
   ```

2. **Generate Build Files**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. **Build the Project**:
   ```bash
   make
   ```

4. **Run the Game**:
   The executable will be available in the `build/` directory:
   ```bash
   ./wordle
   ```

---

## Project Structure

```plaintext
.
├── main.c        # Entry point for the game
├── players.c     # Contains player modes (human, random, AI-based)
├── wordle.c      # Core game logic (word validation, feedback generation)
├── wordle.h      # Header file with declarations and constants
├── wordList.txt  # Dictionary of valid words (required at runtime)
└── CMakeLists.txt # Build configuration for the project
```

---

## Customization

- **Add More Words**: Modify or replace `wordList.txt` with your custom list of valid 5-letter words.
- **Modify Game Rules**: Change constants in `wordle.h` (e.g., word length, maximum attempts).
- **Experiment with AI**: Extend or replace AI strategies in `players.c`.

---

## License

This project is distributed under the MIT License. Contributions and modifications are welcome. :smile:

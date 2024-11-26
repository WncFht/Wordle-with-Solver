# Wordle Game Implementation

## Overview
This is a comprehensive implementation of the Wordle word-guessing game, featuring multiple AI strategies and a testing framework. The project includes both the core game mechanics and several sophisticated AI approaches to solve Wordle puzzles efficiently.

## Features
- Multiple AI solving strategies:
  - Information entropy based approach (fully functional)
  - Decision tree based solution (fully functional)
  - Letter frequency analysis (under maintenance)
  - Minimax optimization (under maintenance)
- Human player interface
- Automated testing framework
- Performance statistics collection
- Comprehensive word validation
- Performance visualization

## File Structure
```
/
├── Core Game Files
│   ├── wordle.h/c          # Core game engine and mechanics
│   ├── player.h/c          # Player strategy implementations
│   └── Makefile            # Build configuration
├── Main Programs
│   ├── main.c             # Basic game runner
│   ├── main_plus.c        # Advanced testing framework
│   └── main_all.c         # Full solution test framework
├── Data Files
│   ├── wordList.txt       # Dictionary of valid words
│   ├── solutionList.txt   # List of possible solutions
│   ├── tree_base.txt      # Base decision tree for AI
│   ├── tree_l.txt         # Lowercase version of tree
│   └── tree_u.txt         # Uppercase version of tree
└── Visualization
    ├── graph.py           # Performance visualization script
    ├── guess_distribution.pdf   # Distribution visualization
    └── performance_metrics.pdf  # Metrics comparison
```

## Setup
1. Clone the repository
2. Ensure you have the required data files:
   - Place `wordList.txt` in the working directory
   - Place `solutionList.txt` in the working directory
   - For AI strategy, place `tree.txt` in the working directory
3. Compile the project:
   ```bash
   gcc -o wordle main.c wordle.c player.c -lm
   ```
   Or for testing framework:
   ```bash
   gcc -o wordle_test main_plus.c wordle.c player.c -lm
   ```
   Or for all words:
    ```bash
   gcc -o wordle_all main_all.c wordle.c player.c -lm
   ```
## Usage
### Basic Game
```bash
./wordle
```
Enter a 5-letter word when prompted.

### Automated Testing
```bash
./wordle_test
```
This will run 30 test cases and provide performance statistics.

### Full Solution Test
```bash
./wordle_all
```
This will test all possible solutions with the selected strategy and provide:
- Success rate statistics
- Score distribution 
- Guess distribution
- Time performance metrics
- Detailed progress monitoring

### Strategy Selection
In `main`, select the strategy by modifying:
```c
Player player = player_entropy;  // or player_AI
```

## AI Strategies

### 1. Entropy-based (player_entropy)
- Uses information theory to maximize information gain
- Starts with "STARE" as first guess
- Considers letter positions and patterns
- Best for optimal average case performance

### 2. Minimax (player_minax)
- Minimizes the maximum possible remaining solutions
- Good for worst-case scenario optimization
- More conservative approach

### 3. Frequency-based (player_frequency)
- Uses letter frequency analysis
- Considers both position-specific and overall frequencies
- Balance between simplicity and effectiveness

### 4. Decision Tree (player_AI)
- Uses pre-computed optimal move sequences
- Starts with "SALET" as first guess
- Most deterministic approach
- Requires `tree_u.txt` file

Note: The frequency-based and minimax strategies are currently under maintenance and will be available in future updates.

## Testing Framework Details

The main_all.c testing framework provides:
- Complete solution space testing
- Progress monitoring (every 100 words)
- Time estimation
- Detailed performance metrics
- Distribution analysis

Example output format:
```
Test 1/2315: APPLE - Score: 8, Guesses: 3
...
Intermediate results after 100 words:
Time elapsed: 43.21 seconds
Average time per word: 0.432 seconds
...
```

## Performance Metrics
The testing framework tracks:
- Success rate (% of solved puzzles)
- Average score (0-10 points per puzzle)
- Average number of guesses needed
- Failed attempts analysis

## Decision Tree AI Strategy (player_AI)
The `player_AI` implementation uses a sophisticated decision tree approach that follows pre-computed optimal move sequences based on previous feedback patterns.

### How it Works
1. Initial Move:
   - Always starts with "SALET" as the first guess
   - This word has been chosen based on extensive analysis of letter frequencies and information gain

2. Pattern Tracking:
   - Maintains a cumulative pattern of guesses and feedback
   - Format: `<WORD> <PATTERN><LEVEL> ...`
   - Example: `SALET GYBBG1 CRANE GBBBY2`

3. Decision Making:
   - Consults `tree.txt` for next optimal move
   - Each line in tree.txt represents a successful solution path
   - Matches current game state against known patterns
   - Chooses the statistically best next word

### tree.txt Format
```
salet BBBBB1 courd BBBBB2 nymph BBBBY3 whiff GGGGG4
salet BBBBB1 courd BBBBB2 nymph BGYYB3 pygmy GGGGG4
salet BBBBB1 courd BBBBB2 nymph BYBBB3 fizzy GGGGG4
salet BBBBB1 courd BBBBB2 nymph BYBBB3 fizzy YGBBG4 jiffy GGGGG5
...
```
Each line represents:
- Words played in sequence
- Feedback received (G=green, Y=yellow, B=black)
- Level number for each guess

### Advantages
- Deterministic behavior
- Pre-computed optimal moves
- Fast decision making
- High success rate
- Consistent performance

### Limitations
- Requires tree.txt file
- Memory intensive
- Fixed initial strategy
- Limited adaptation to unknown patterns

### Performance Compared

#### One-Layer-Entropy AI

```
Final Test Summary:
Total time: 1001.03 seconds
Average time per word: 0.432 seconds
Total words: 2315
Successful tests: 2315
Success rate: 100.00%
Average score: 6.76
Average guesses: 4.24

Guess Distribution:
1 guesses: 1 words (0.04%)
2 guesses: 11 words (0.48%)
3 guesses: 332 words (14.34%)
4 guesses: 1194 words (51.58%)
5 guesses: 660 words (28.51%)
6 guesses: 109 words (4.71%)
7 guesses: 7 words (0.30%)
8 guesses: 0 words (0.00%)
9 guesses: 1 words (0.04%)
10 guesses: 0 words (0.00%)
```

#### Decision-Tree-Based AI

```
Final Test Summary:
Total time: 1.25 seconds
Average time per word: 0.001 seconds
Total words: 2315
Successful tests: 2315
Success rate: 100.00%
Average score: 7.58
Average guesses: 3.42

Guess Distribution:
1 guesses: 0 words (0.00%)
2 guesses: 78 words (3.37%)
3 guesses: 1225 words (52.92%)
4 guesses: 971 words (41.94%)
5 guesses: 41 words (1.77%)
6 guesses: 0 words (0.00%)
7 guesses: 0 words (0.00%)
8 guesses: 0 words (0.00%)
9 guesses: 0 words (0.00%)
10 guesses: 0 words (0.00%)
```
## Note
The current version focuses on the **entropy-based** and **decision tree** approaches. Other strategies (frequency and minimax) are being updated for improved performance and will be available in future releases.

## License

This project is distributed under the MIT License. Contributions and modifications are welcome. :smile:

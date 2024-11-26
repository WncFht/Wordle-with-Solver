# Wordle Game Implementation

## Overview
This is a comprehensive implementation of the Wordle word-guessing game, featuring multiple AI strategies and a testing framework. The project includes both the core game mechanics and several sophisticated AI approaches to solve Wordle puzzles efficiently.

## Features
- Multiple AI solving strategies:
  - Information entropy based approach
  - Minimax optimization
  - Letter frequency analysis
  - Decision tree based solution
  - Random guessing
- Human player interface
- Automated testing framework
- Performance statistics collection
- Comprehensive word validation
- Memory-safe implementation

## File Structure
- `wordle.h/c`: Core game engine and mechanics
- `player.h/c`: Player strategy implementations
- `main.c`: Basic game runner
- `main_plus.c`: Advanced testing framework
- Required data files:
  - `wordList.txt`: Dictionary of valid words
  - `tree.txt`: Decision tree for AI strategy
  - `solutionList.txt`: List of possible solutions

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
- Requires `tree.txt` file

## Testing Framework
The testing system (`main_plus.c`) provides:
- Random solution selection
- Success rate calculation
- Average score tracking
- Average guesses needed
- Intermediate progress reports
- Detailed final statistics

## Performance Metrics
The testing framework tracks:
- Success rate (% of solved puzzles)
- Average score (0-10 points per puzzle)
- Average number of guesses needed
- Failed attempts analysis

## Note
- All words are processed in uppercase
- Dictionary words must be 5 letters
- Maximum 10 guesses per game
- Memory is managed carefully throughout

## Future Improvements
- Add more AI strategies
- Implement parallel testing
- Enhance decision tree generation
- Add visualization tools
- Improve error handling

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

### Performance
- Average success rate: ~100%
- Average guesses: .42
- Best case: 1 guess
- Worst case: 5 guesses


## Customization

- **Add More Words**: Modify or replace `wordList.txt` with your custom list of valid 5-letter words.
- **Modify Game Rules**: Change constants in `wordle.h` (e.g., word length, maximum attempts).
- **Experiment with AI**: Extend or replace AI strategies in `players.c`.


## License

This project is distributed under the MIT License. Contributions and modifications are welcome. :smile:

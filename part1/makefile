CC = gcc
CFLAGS = -Wall -Wextra
TARGET = wordle
OBJS = wordle.o main.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

wordle.o: wordle.c wordle.h
	$(CC) $(CFLAGS) -c wordle.c

main.o: main.c wordle.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) $(TARGET)
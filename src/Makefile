CC = cc
CFLAGS = -ggdb -Wextra -Wall -pedantic

SRCS = main.c vector.c lexer.c token.c parser.c arena.c
OBJS = $(SRCS:.c=.o)
DEPS = lexer.h token.h parser.h arena.h ast.h vector.h

TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

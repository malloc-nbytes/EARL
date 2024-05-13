CC = cc
CFLAGS = -std=c99 -Wextra -Wall -pedantic
DEBUG ?= 1

ifeq ($(DEBUG), 1)
CFLAGS += -ggdb -DDEBUG -Wswitch-default
endif

SRCS = main.c lexer.c token.c parser.c arena.c vector.c utils.c
OBJS = $(SRCS:.c=.o)
DEPS = lexer.h token.h parser.h arena.h ast.h vector.h utils.h

TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

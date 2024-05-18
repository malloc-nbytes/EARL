CC = cc
CFLAGS = -std=c99 -pedantic
PROFILE ?= debug
INC_DIRS = -I. -Itest

ifeq ($(filter clean, $(MAKECMDGOALS)),)
ifeq ($(PROFILE), debug)
CFLAGS += -ggdb -DDEBUG -Wextra -Wall -Werror -Wswitch-default
$(info Building in debugging mode. Switch off with `make PROFILE=release`)
else ifeq ($(PROFILE), release)
CFLAGS += -march=native -O2
$(info Building in release mode.)
else
$(error invalid PROFILE. Must be set to either "debug" or "release")
endif
endif

SRCS = main.c lexer.c token.c parser.c arena.c ast.c vector.c utils.c hashtbl.c pair.c
OBJS = $(SRCS:.c=.o)
DEPS = lexer.h token.h parser.h arena.h ast.h vector.h utils.h notify.h hashtbl.h pair.h

TARGET = main

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

test: test-suite
	./test-suite

test-suite: test-suite.o test-vec.o test-pair.o $(filter-out main.o, $(OBJS))
	$(CC) $(CFLAGS) $^ -o $@

test-vec.o: test/vec/test-vec.c test/vec/test-vec.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

test-pair.o: test/pair/test-pair.c test/pair/test-pair.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

test-suite.o: test/test-suite.c test/test-suite.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) test-suite test-vec.o test-pair.o test-suite.o

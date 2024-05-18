# Compiler and flags
CC = cc
CFLAGS = -std=c99 -pedantic -Wno-gnu-zero-variadic-macro-arguments
PROFILE ?= debug
INC_DIRS = -Iinclude -I. -Itest

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
OBJ_DIR = obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS = include/lexer.h include/token.h include/parser.h include/arena.h include/ast.h include/vector.h include/utils.h include/notify.h include/hashtbl.h include/pair.h

TARGET = main

.PHONY: all clean test

all: $(TARGET)

# Rule to create the obj directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

$(OBJ_DIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

test: test-suite
	./test-suite

test-suite: $(OBJ_DIR)/test-suite.o $(OBJ_DIR)/test-vec.o $(OBJ_DIR)/test-pair.o $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/test-vec.o: test/vec/test-vec.c test/vec/test-vec.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(OBJ_DIR)/test-pair.o: test/pair/test-pair.c test/pair/test-pair.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(OBJ_DIR)/test-suite.o: test/test-suite.c test/test-suite.h $(DEPS)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) test-suite

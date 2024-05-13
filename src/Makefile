CC = cc
CFLAGS = -std=c99 -pedantic
PROFILE ?= debug

ifeq ($(filter clean, $(MAKECMDGOALS)),)
ifeq ($(PROFILE), debug)
CFLAGS += -ggdb -DDEBUG -Wextra -Wall -Werror -Wswitch-default
$(info Building in debugging mode. Switch off with `make PROFILE=release`)
else ifeq ($(PROFILE), release)
CFLAGS += -O3
$(info Building in release mode.)
else
$(error invalid PROFILE. Must be set to either "debug" or "release")
endif
endif

SRCS = main.c lexer.c token.c parser.c arena.c ast.c vector.c utils.c hashtbl.c
OBJS = $(SRCS:.c=.o)
DEPS = lexer.h token.h parser.h arena.h ast.h vector.h utils.h notify.h hashtbl.h

TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

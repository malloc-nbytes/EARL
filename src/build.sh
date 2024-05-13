#!/bin/bash
set -xe
cc -ggdb -Wextra -Wall -pedantic -o main main.c lexer.c arena.c token.c

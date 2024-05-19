AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.c arena.c ast.c hashtbl.c lexer.c pair.c parser.c token.c utils.c vector.c

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

# EXTRA_CFLAGS = -std=c99 -pedantic -ggdb -Wall -Wextra -Wextra -DDEBUG

AM_CFLAGS = -O2 -Wall -Werror -Wextra -pedantic -std=c99 -g -DDEBUG

.PHONY: test
test:
	$(MAKE) -C test test_suite
	./test/test_suite

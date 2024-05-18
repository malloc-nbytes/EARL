AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.c arena.c ast.c hashtbl.c lexer.c pair.c parser.c token.c utils.c vector.c

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

# Add the 'test' directory
SUBDIRS = test

EXTRA_CFLAGS = -Wall -Wextra -Wextra

# Define a phony target 'test' to build and run test_suite
.PHONY: test
test:
	$(MAKE) -C test test_suite
	./test/test_suite

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.cpp arena.cpp ast.cpp hashtbl.cpp lexer.cpp pair.cpp parser.cpp token.cpp utils.cpp vector.cpp interpreter.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

# Flags for C compiler
AM_CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -DDEBUG -Wno-gnu-zero-variadic-macro-arguments -O0

# Flags for C++ compiler
AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g -DDEBUG

.PHONY: test
test:
	$(MAKE) -C test test_suite
	./test/test_suite

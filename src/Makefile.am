AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.cpp arena.cpp ast.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp earlvar.cpp earlty.cpp interpreter.cpp intrinsics.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -g -O0 -DDEBUG

.PHONY: test
test:
	$(MAKE) -C test test_suite
	./test/test_suite

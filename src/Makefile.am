AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.cpp arena.cpp ast.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp interpreter.cpp intrinsics.cpp err.cpp function.cpp evalres.cpp value.cpp variable.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -g -O0 -DDEBUG

.PHONY: test docs all
test:
	$(MAKE) -C test test_suite
	./test/test_suite

docs:
	doxygen Doxyfile
	-$(MAKE) -C ../docs/latex/
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: docs main test

.DEFAULT_GOAL = main

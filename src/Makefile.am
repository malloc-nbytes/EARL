AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.cpp arena.cpp ast.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp interpreter.cpp intrinsics.cpp err.cpp earl-function.cpp earl-variable.cpp earl-value.cpp earl-types.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -g -O0 -DDEBUG

.PHONY: test docs all
test: main
	$(MAKE) -C test test_suite
	@echo "Running interpreter tests"
	./test/test_suite
	@echo "Running EARL tests"
	./main ./test/earl-tests/main.earl

docs:
	doxygen Doxyfile
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: docs main test

.DEFAULT_GOAL = main

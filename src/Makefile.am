AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = main
main_SOURCES = main.cpp arena.cpp ast.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp interpreter.cpp intrinsics.cpp err.cpp function.cpp variable.cpp types.cpp int.cpp str.cpp void.cpp list.cpp module.cpp class.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/test

SUBDIRS = test

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -Wno-unused-variable

.PHONY: test docs all
test: main
	$(MAKE) -C test test_suite
	@echo "Running interpreter tests"
	./test/test_suite
	@echo "Running EARL tests"
	make
	cd ./test/earl-tests/ && \
	../../main ./main.earl

vtest: main
	$(MAKE) -C test test_suite
	@echo "Running interpreter tests"
	./test/test_suite
	@echo "Running EARL tests"
	make
	cd ./test/earl-tests/ && \
	valgrind ../../main ./main.earl

docs:
	doxygen Doxyfile
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: docs main test

.DEFAULT_GOAL = main

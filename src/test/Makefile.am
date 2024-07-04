AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite

test_suite_SOURCES = ../list-intrinsics.cpp ../str-intrinsics.cpp ../option-intrinsics.cpp ../char-intrinsics.cpp ../file-intrinsics.cpp ../closure.cpp ../option.cpp ../this.cpp ../file.cpp ../char.cpp ../bool.cpp ../class.cpp ../int.cpp ../str.cpp ../void.cpp ../list.cpp ../module.cpp ../types.cpp ../function.cpp ../variable.cpp ../err.cpp ../arena.cpp ../ast.cpp ../lexer.cpp ../token.cpp ../utils.cpp ../ctx.cpp ../parser.cpp ../interpreter.cpp ../intrinsics.cpp test-suite.cpp parsers/test-parsers.cpp loops/test-loops.cpp conditionals/test-conditionals.cpp functions/test-functions.cpp

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

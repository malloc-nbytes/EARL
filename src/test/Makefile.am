AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite

test_suite_SOURCES = ../member-intrinsics/list-intrinsics.cpp ../member-intrinsics/str-intrinsics.cpp ../member-intrinsics/option-intrinsics.cpp ../member-intrinsics/char-intrinsics.cpp ../member-intrinsics/file-intrinsics.cpp ../primitives/closure.cpp ../primitives/option.cpp ../primitives/this.cpp ../primitives/file.cpp ../primitives/char.cpp ../primitives/bool.cpp ../primitives/class.cpp ../primitives/int.cpp ../primitives/str.cpp ../primitives/void.cpp ../primitives/list.cpp ../primitives/module.cpp ../types.cpp ../function.cpp ../variable.cpp ../err.cpp ../arena.cpp ../ast.cpp ../lexer.cpp ../token.cpp ../utils.cpp ../ctx.cpp ../parser.cpp ../interpreter.cpp ../intrinsics.cpp test-suite.cpp parsers/test-parsers.cpp loops/test-loops.cpp conditionals/test-conditionals.cpp functions/test-functions.cpp

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

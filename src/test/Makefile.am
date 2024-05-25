AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite

test_suite_SOURCES = ../arena.cpp ../ast.cpp ../lexer.cpp ../token.cpp ../utils.cpp test-suite.cpp lexer/test-lexer.cpp

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

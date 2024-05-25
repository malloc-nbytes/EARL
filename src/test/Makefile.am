AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite

test_suite_SOURCES = ../arena.cpp ../ast.cpp ../hashtbl.cpp ../lexer.cpp ../pair.cpp ../parser.cpp ../token.cpp ../utils.cpp ../vector.cpp test-suite.cpp pair/test-pair.cpp vector/test-vector.cpp parsers/test-parsers.cpp hashtbl/test-hashtbl.cpp lexer/test-lexer.cpp

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite

test_suite_SOURCES = ../arena.c ../ast.c ../hashtbl.c ../lexer.c ../pair.c ../parser.c ../token.c ../utils.c ../vector.c test-suite.c pair/test-pair.c vec/test-vec.c parsers/test-parsers.c --help/test---help.c

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

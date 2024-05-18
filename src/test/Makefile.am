AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = test_suite
test_suite_SOURCES = test-suite.c pair/test-pair.c vec/test-vec.c ../arena.c ../ast.c ../hashtbl.c ../lexer.c ../pair.c ../parser.c ../token.c ../utils.c ../vector.c

.PHONY: earl_test_suite
earl_test_suite: $(bin_PROGRAMS)
	./test_suite

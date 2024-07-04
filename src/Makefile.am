AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = earl
earl_SOURCES = main.cpp arena.cpp ast.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp interpreter.cpp intrinsics.cpp err.cpp function.cpp variable.cpp types.cpp primitives/int.cpp primitives/str.cpp primitives/void.cpp primitives/list.cpp primitives/module.cpp primitives/class.cpp primitives/bool.cpp primitives/char.cpp primitives/file.cpp primitives/this.cpp primitives/option.cpp primitives/closure.cpp member-intrinsics/file-intrinsics.cpp member-intrinsics/char-intrinsics.cpp member-intrinsics/option-intrinsics.cpp member-intrinsics/str-intrinsics.cpp member-intrinsics/list-intrinsics.cpp

include_HEADERS = std/io.earl std/linked-list.earl std/list.earl std/math.earl std/queue.earl std/set.earl std/stack.earl std/str.earl std/utils.earl

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -Wno-unused-variable

includedir = $(prefix)/include/EARL/std

$(includedir):
	mkdir -p $(includedir)

install-data-local: $(include_HEADERS)
	$(mkinstalldirs) $(DESTDIR)$(includedir)

.PHONY: test docs all

test: earl
	@echo "Running tests, make sure that the stdlib is installed"
	cd test/earl-tests && ../../earl ./main.earl

docs:
	doxygen Doxyfile
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: earl docs test

.DEFAULT_GOAL = earl

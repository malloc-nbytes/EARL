AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = earl
earl_SOURCES = main.cpp arena.cpp lexer.cpp token.cpp utils.cpp parser.cpp ctx.cpp interpreter.cpp intrinsics.cpp err.cpp function.cpp variable.cpp types.cpp primitives/int.cpp primitives/str.cpp primitives/void.cpp primitives/list.cpp primitives/module.cpp primitives/class.cpp primitives/bool.cpp primitives/char.cpp primitives/file.cpp primitives/this.cpp primitives/option.cpp primitives/closure.cpp primitives/break.cpp member-intrinsics/file-intrinsics.cpp member-intrinsics/char-intrinsics.cpp member-intrinsics/option-intrinsics.cpp member-intrinsics/str-intrinsics.cpp member-intrinsics/list-intrinsics.cpp grammar/expr-array-access.cpp grammar/expr-binary.cpp grammar/expr-bool.cpp grammar/expr-charlit.cpp grammar/expr-closure.cpp grammar/expr-funccall.cpp grammar/expr-get.cpp grammar/expr-ident.cpp grammar/expr-intlit.cpp grammar/expr-listlit.cpp grammar/expr-none.cpp grammar/expr-strlit.cpp grammar/program.cpp grammar/stmt-block.cpp grammar/stmt-class.cpp grammar/stmt-def.cpp grammar/stmt-expr.cpp grammar/stmt-for.cpp grammar/stmt-if.cpp grammar/stmt-import.cpp grammar/stmt-let.cpp grammar/stmt-match.cpp grammar/stmt-mod.cpp grammar/stmt-mut.cpp grammar/stmt-return.cpp grammar/stmt-while.cpp grammar/stmt-break.cpp

include_HEADERS = std/io.earl std/linked-list.earl std/list.earl std/math.earl std/queue.earl std/set.earl std/stack.earl std/str.earl std/utils.earl std/os.earl

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -Wno-unused-variable

includedir = $(prefix)/include/EARL/std

.PHONY: test docs all install-data-hook

$(includedir):
	mkdir -p $(includedir)

install-data-local: $(include_HEADERS)
	$(mkinstalldirs) $(DESTDIR)$(includedir)

install-data-hook:
	@echo "Do you want to install earlmngr? [y/N] " && read ans && [ $${ans:-N} = y ]
	@if [ $$? -eq 0 ]; then \
		echo "Installing earlmngr..."; \
		wget https://raw.githubusercontent.com/malloc-nbytes/earlmngr/main/src/earlmngr.earl -O earlmngr; \
		chmod +x earlmngr; \
		sudo mv earlmngr $(prefix)/bin/; \
		echo "earlmngr installed to $(prefix)/bin/"; \
	else \
		echo "earlmngr installation skipped."; \
	fi

test: earl
	@echo "Running tests, make sure that the stdlib is installed"
	cd test/earl-tests && ../../earl ./main.earl

docs:
	doxygen Doxyfile
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: earl docs test

.DEFAULT_GOAL = earl


AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = earl
earl_SOURCES = main.cpp interpreter-evaluation-result.cpp closure-ctx.cpp world-ctx.cpp function-ctx.cpp class-ctx.cpp arena.cpp lexer.cpp token.cpp utils.cpp parser.cpp interpreter.cpp intrinsics.cpp err.cpp function.cpp variable.cpp types.cpp primitives/int.cpp primitives/float.cpp primitives/str.cpp primitives/void.cpp primitives/list.cpp primitives/module.cpp primitives/bool.cpp primitives/char.cpp primitives/file.cpp primitives/option.cpp primitives/closure.cpp primitives/break.cpp primitives/class.cpp member-intrinsics/file-intrinsics.cpp member-intrinsics/char-intrinsics.cpp member-intrinsics/option-intrinsics.cpp member-intrinsics/str-intrinsics.cpp member-intrinsics/list-intrinsics.cpp grammar/expr-array-access.cpp grammar/expr-binary.cpp grammar/expr-bool.cpp grammar/expr-charlit.cpp grammar/expr-closure.cpp grammar/expr-funccall.cpp grammar/expr-get.cpp grammar/expr-ident.cpp grammar/expr-floatlit.cpp grammar/expr-intlit.cpp grammar/expr-listlit.cpp grammar/expr-mod-access.cpp grammar/expr-none.cpp grammar/expr-strlit.cpp grammar/expr-tuple.cpp grammar/program.cpp grammar/stmt-block.cpp grammar/stmt-class.cpp grammar/stmt-def.cpp grammar/stmt-expr.cpp grammar/stmt-for.cpp grammar/stmt-if.cpp grammar/stmt-import.cpp grammar/stmt-let.cpp grammar/stmt-match.cpp grammar/stmt-mod.cpp grammar/stmt-mut.cpp grammar/stmt-return.cpp grammar/stmt-while.cpp grammar/stmt-break.cpp include/shared-scope.hpp

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
	@echo "Installation of earlmngr is currently WIP"
	@echo "Do you want to install earlmngr? [y/N] " && read ans && [ $${ans:-N} = y ]
	@if [ $$? -eq 0 ]; then \
		echo "Installing earlmngr..."; \
		wget https://raw.githubusercontent.com/malloc-nbytes/earlmngr/main/src/earlmngr.earl -O earlmngr; \
		chmod +x earlmngr; \
		sudo mv earlmngr $(prefix)/bin/; \
		echo "earlmngr installed to $(prefix)/bin/"; \
	else \
		echo "earlmngr installation skipped."; \
		exit 0; \
	fi

uninstall-hook:
	@if [ -f $(prefix)/bin/earlmngr ]; then \
		echo "Uninstalling earlmngr..."; \
		sudo rm -f $(prefix)/bin/earlmngr; \
		echo "earlmngr uninstalled from $(prefix)/bin/"; \
	else \
		echo "earlmngr not found in $(prefix)/bin/"; \
	fi

test: earl
	@echo "Running tests, make sure that the stdlib is installed"
	cd test/earl-tests && ../../earl ./main.earl

debug: clean
	@echo "Building EARL in debug mode"
	$(MAKE) CXXFLAGS='-O0 -DDEBUG -ggdb' CXX='$(CXX)'
	mv ./earl earl-debug-build

docs:
	doxygen Doxyfile
	@echo "--- Documentation generated in src/docs/html/index.html ---"

all: earl docs test

.DEFAULT_GOAL = earl

AC_INIT([main], [1.0])

AC_CONFIG_SRCDIR([main.cpp])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C++ compiler
AC_LANG([C++])
AC_PROG_CXX

# Check for C compiler
AC_PROG_CC

# Check for doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen], [no])
if test "x$DOXYGEN" = "xno"; then
  AC_MSG_WARN([Doxygen is required to build the documentation. Please install it.])
fi

# Check for valgrind
AC_CHECK_PROG([VALGRIND], [valgrind], [valgrind], [no])
if test "x$VALGRIND" = "xno"; then
  AC_MSG_WARN([Valgrind is required for running `vtests`. Please install it.])
fi

# Tells automake to create Makefiles in specified directories
AC_CONFIG_FILES([Makefile
                test/Makefile])

AC_OUTPUT

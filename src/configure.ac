AC_INIT([earl], [1.0], [zdhdev@yahoo.com])

AC_CONFIG_SRCDIR([main.cpp])
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_HEADERS([include/config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C++ compiler
AC_LANG([C++])
AC_PROG_CXX

# Check for C compiler
AC_PROG_CC

# Check for doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen], [no])
if test "x$DOXYGEN" = "xno"; then
  AC_MSG_WARN([---Doxygen is not installed. Is required to build the source code documentation.---])
fi

# Tells automake to create Makefiles in specified directories
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_INIT([earl], [1.0], [zdhdev@yahoo.com])

AC_CONFIG_SRCDIR([main.cpp])
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_HEADERS([include/config.h])

AC_ARG_WITH([prefix], [AS_HELP_STRING([--prefix=DIR], [install architecture-independent files in DIR])],
  [prefix="$withval"], [prefix="${ac_default_prefix}"])
AC_DEFINE_UNQUOTED([PREFIX], ["$prefix"], [Define to the installation prefix])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C++ compiler
AC_LANG([C++])
AC_PROG_CXX

# Check for C compiler
AC_PROG_CC

AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([filesystem])
AC_CHECK_HEADERS([optional])
AC_CHECK_HEADERS([cassert])
AC_CHECK_HEADERS([algorithm])
AC_CHECK_HEADERS([fstream])
AC_CHECK_HEADERS([sstream])
AC_CHECK_HEADERS([unordered_map])
AC_CHECK_HEADERS([vector])
AC_CHECK_HEADERS([memory])
AC_CHECK_HEADERS([cstdlib])
AC_CHECK_HEADERS([exception])
AC_CHECK_HEADERS([functional])

AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([stdbool.h])
AC_CHECK_HEADERS([stdarg.h.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/stat.h])

# Check for doxygen
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen], [no])
if test "x$DOXYGEN" = "xno"; then
  AC_MSG_WARN([---Doxygen is not installed. Is required to build the source code documentation.---])
fi

# Tells automake to create Makefiles in specified directories
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

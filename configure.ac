AC_PREREQ(2.61)

AC_INIT([fstorage],[0.5.1],[lukasz@fstorage.pl])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT("/usr")

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checking header files
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset pow strdup strerror strtol strchr])

COMMONFLAGS="-Wall -static-libgcc -Wno-format-y2k -W -Wpointer-arith -Wreturn-type -Wcast-qual -Wwrite-strings -Wswitch -Wshadow -Wcast-align -Wchar-subscripts -Winline -Wuninitialized -O2 -Wredundant-decls -Wcomment -Wformat -Wimplicit -Wlong-long -Wno-import -Wparentheses -Wpointer-arith -Wreturn-type -Wshadow -Wswitch -Wtrigraphs -Wwrite-strings -ggdb -Wunused"

CXXFLAGS="$COMMONFLAGS -Wno-variadic-macros -Wno-format -std=gnu++98"

AC_SUBST(CXXFLAGS)

CFLAGS="$COMMONFLAGS -pedantic -Waggregate-return -Wmissing-prototypes -Wbad-function-cast -Wnested-externs -Wimplicit-int -Wimplicit-function-declaration -Wmain -Wmissing-declarations -std=c99" 

AC_SUBST(CFLAGS)

LDFLAGS="-L/usr/local/lib -R/usr/local/lib"

AC_CONFIG_FILES([
Makefile
src/Makefile
src/libfstorage/Makefile
src/libfstorage++/Makefile
src/fstoraged/Makefile
src/btp/Makefile
src/common/Makefile
src/tests/Makefile
])

AC_OUTPUT

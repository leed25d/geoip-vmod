AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2011 Varnish Software AS])
AC_INIT([libvmod-geoip], [trunk])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/vmod_geoip.vcc)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_SYSTEM
AC_LANG(C)

AM_INIT_AUTOMAKE([foreign])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
	AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_CPP

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Check for rst utilities
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py], "no")
if test "x$RST2MAN" = "xno"; then
	AC_MSG_WARN([rst2man not found - not building man pages])
fi
AM_CONDITIONAL(HAVE_RST2MAN, [test "x$RST2MAN" != "xno"])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/stdlib.h])

# Check for python
AC_CHECK_PROGS(PYTHON, [python3 python3.1 python3.2 python2.7 python2.6 python2.5 python2 python], [AC_MSG_ERROR([Python is needed to build this vmod, please install python.])])

# Varnish include files tree
VARNISH_VMOD_INCLUDES
VARNISH_VMOD_DIR
VARNISH_VMODTOOL

AC_PATH_PROG([VARNISHTEST], [varnishtest])
AC_PATH_PROG([VARNISHD], [varnishd])

# Check for GeoIP
AC_ARG_WITH([geoip],
    [  --with-geoip=PATH       find the GeoIP SDK installed in PATH],
    [
        CPPFLAGS="$CPPFLAGS -I${withval}/include"
        LDFLAGS="$LDFLAGS -L${withval}/lib"
    ], 
    [:]
)
AC_CHECK_LIB([GeoIP], [GeoIP_assign_region_by_inetaddr], 
    [AC_CHECK_HEADER([GeoIP.h],
        [],
        [AC_MSG_ERROR([Failed to find the GeoIP SDK (1).  Specify --with-geoip=PATH])]
    )],
    [AC_MSG_ERROR([Failed to find the GeoIP SDK (2).  Specify --with-geoip=PATH])]
)
LIBS="$LIBS -lGeoIP"

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	man/Makefile
])
AC_OUTPUT

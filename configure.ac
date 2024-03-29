# Copyright (C) 2014, 2015, 2016, 2019, 2022, 2023  Andre Colomb
#
# This file is part of elf-mangle.
#
# This file is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# elf-mangle is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program.  If not, see
# <http://www.gnu.org/licenses/>.


AC_INIT([elf-mangle],[1.2],[src@andre.colomb.de])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/elf-mangle.c])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])


# Checks for programs.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC

# Use GNU Gnulib if required macros are present
m4_ifdef([gl_EARLY],
   [gl_EARLY
    gnulib=1], [gnulib=0])
AM_CONDITIONAL([USE_GNULIB], [test "x$gnulib" = x1])

# More checks for programs.
AM_PROG_AR
LT_INIT


# Checks for libraries.
m4_ifdef([gl_INIT], [gl_INIT])
AC_SEARCH_LIBS([elf_version], [elf], [],
   [AC_MSG_ERROR([Libelf is needed to compile this software,
                  but no library could be found.
                  Specify LDFLAGS and / or LIBS if necessary.
                  Hint: `pkg-config --libs libelf`
                        may be of help if installed.])])
AC_CHECK_HEADERS([gelf.h], [],
   [AC_MSG_ERROR([Libelf is needed to compile this software,
                  but no working headers could be found.
                  Specify CPPFLAGS if necessary.
                  Hint: `pkg-config --cflags libelf`
                        may be of help if installed.])])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_NEED([need-formatstring-macros])
AM_GNU_GETTEXT_VERSION([0.19.7])

# Use installed libcintelhex or fall back to embedded copy
cintelhex=0
cintelhex_internal=0
AC_MSG_CHECKING([for libcintelhex])
AC_ARG_WITH([cintelhex],
   [AS_HELP_STRING([--with-cintelhex=PFX],
       [base of libcintelhex installation [AUTO-DETECT].
        Use "internal" for embedded copy])],
   [AC_MSG_RESULT([$with_cintelhex])
    AS_CASE([$with_cintelhex],
       [no],       [],
       [yes],      [AC_CHECK_HEADERS([cintelhex.h],
                       [AC_CHECK_LIB([cintelhex], [ihex_byte_copy],
                           [cintelhex=1], [cintelhex_internal=1])],
                       [cintelhex_internal=1])],
       [internal], [cintelhex_internal=1],
       [AC_SUBST([CINTELHEX_CPPFLAGS], [-I$with_cintelhex/include])
        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CINTELHEX_CPPFLAGS $CPPFLAGS"
        AC_CHECK_HEADERS([cintelhex.h],
           [AC_SUBST([CINTELHEX_LDFLAGS], [-L$with_cintelhex/lib])
	    saved_LDFLAGS="$LDFLAGS"
            LDFLAGS="$CINTELHEX_LDFLAGS $LDFLAGS"
            AC_CHECK_LIB([cintelhex], [ihex_byte_copy],
               [cintelhex=1])
	    LDFLAGS="$saved_LDFLAGS"])
        CPPFLAGS="$saved_CPPFLAGS"
       ])
   ],
   [AC_MSG_RESULT([--with-cintelhex not specified])
    AC_CHECK_HEADERS([cintelhex.h],
       [AC_CHECK_LIB([cintelhex], [ihex_byte_copy],
           [cintelhex=1], [cintelhex_internal=1])],
       [cintelhex_internal=1])
   ])

AS_IF([test "x$cintelhex_internal" = x1],
   [AC_MSG_NOTICE([Trying embedded internal copy of libcintelhex])
    AS_UNSET([ac_cv_header_cintelhex_h])
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="-I${srcdir}/libcintelhex/include $CPPFLAGS"
    AC_CHECK_HEADERS([cintelhex.h],
       [cintelhex=1],
       [cintelhex_internal=0])
    CPPFLAGS="$saved_CPPFLAGS"
   ],
   [AS_IF([test x$cintelhex = x1], AC_SUBST([CINTELHEX_LIBS], [-lcintelhex]))])
AM_CONDITIONAL([HAVE_INTELHEX], [test x$cintelhex = x1])
AM_CONDITIONAL([HAVE_INTELHEX_INTERNAL], [test x$cintelhex_internal = x1])
AC_DEFINE_UNQUOTED([HAVE_INTELHEX], [$cintelhex], [Define if you have libcintelhex])
AM_COND_IF([HAVE_INTELHEX],
   [AC_MSG_NOTICE([Including support for Intel Hex format files.])],
   [AC_MSG_WARN([Intel Hex format files will not be supported.])])


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([libintl.h])



# Checks for typedefs
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_OFF_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T



# Checks for library functions.
AC_CHECK_FUNCS([ftruncate])
AC_CHECK_FUNCS([munmap])
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([malloc])
AC_CHECK_FUNCS([realloc])
AC_FUNC_MMAP



# Checks for optional feature requests
AC_MSG_CHECKING([whether extension module for custom options should be built])
AC_ARG_ENABLE([custom-options],
   [AS_HELP_STRING([--enable-custom-options],
       [Build extension module with custom options])],
   [AS_CASE([$enableval],
       [yes], [custom_options=yes
               AC_DEFINE([WITH_CUSTOM_OPTIONS], [1],
                         [Building extension module with custom options])],
       [no],  [custom_options=no],
       [AC_MSG_ERROR([bad value ${enableval} for --enable-custom-options])])
   ],
   [custom_options=no])
AC_MSG_RESULT([${custom_options}])
AM_CONDITIONAL([CUSTOM_OPTIONS], [test "x$custom_options" = xyes])


AC_MSG_CHECKING([whether extension module for custom known fields should be built])
AC_ARG_ENABLE([custom-fields],
   [AS_HELP_STRING([--enable-custom-fields],
       [Build extension module with custom known fields])],
   [AS_CASE([$enableval],
       [yes], [custom_fields=yes
               AC_DEFINE([WITH_CUSTOM_FIELDS], [1],
                         [Building extension module with custom known fields])],
       [no],  [custom_fields=no],
       [AC_MSG_ERROR([bad value ${enableval} for --enable-custom-fields])])
   ],
   [custom_fields=no])
AC_MSG_RESULT([${custom_fields}])
AM_CONDITIONAL([CUSTOM_FIELDS], [test "x$custom_fields" = xyes])


AC_MSG_CHECKING([whether extension module for custom post-processors should be built])
AC_ARG_ENABLE([custom-post-process],
   [AS_HELP_STRING([--enable-custom-post-process],
       [Build extension module with custom post-processors])],
   [AS_CASE([$enableval],
       [yes], [custom_post_process=yes
               AC_DEFINE([WITH_CUSTOM_POST_PROCESS], [1],
                         [Building extension module with custom post-processors])],
       [no],  [custom_post_process=no],
       [AC_MSG_ERROR([bad value ${enableval} for --enable-custom-post-process])])
   ],
   [custom_post_process=no])
AC_MSG_RESULT([${custom_post_process}])
AM_CONDITIONAL([CUSTOM_POST_PROCESS], [test "x$custom_post_process" = xyes])


# Ouput definitions
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile Doxyfile src/Makefile po/Makefile.in])
AM_COND_IF([HAVE_INTELHEX_INTERNAL],
   [AC_CONFIG_SUBDIRS([libcintelhex])])
m4_ifdef([gl_INIT],
   [AC_CONFIG_FILES([gnulib/Makefile])])
AC_OUTPUT

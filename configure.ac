# Copyright (C) 2014  Andre Colomb
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


AC_INIT([elf-mangle], [0.1], [src@andre.colomb.de])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/elf-mangle.c])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC_C99
AC_PROG_RANLIB
AM_PROG_AR

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_FUNC_MMAP
AC_CHECK_LIB([elf], [elf_version])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_NEED([need-formatstring-macros])
AM_GNU_GETTEXT_VERSION([0.18.3])

AC_MSG_CHECKING([whether extension module for custom options should be built])
AC_ARG_ENABLE([custom-options],
  [AS_HELP_STRING([--enable-custom-options],
                  [Build extension module with custom options])],
  [case "${enableval}" in
     yes) custom_options=true ;;
     no)  custom_options=false ;;
     *)   AC_MSG_ERROR([bad value ${enableval} for --enable-custom-options]) ;;
   esac],
  [custom_options=false])
AC_MSG_RESULT([${enable_custom-options}])
AM_CONDITIONAL([CUSTOM_OPTIONS], [test "x$custom_options" = xtrue])


AC_MSG_CHECKING([whether extension module for custom known fields should be built])
AC_ARG_ENABLE([custom-fields],
  [AS_HELP_STRING([--enable-custom-fields],
                  [Build extension module with custom known fields])],
  [case "${enableval}" in
     yes) custom_fields=true ;;
     no)  custom_fields=false ;;
     *)   AC_MSG_ERROR([bad value ${enableval} for --enable-custom-fields]) ;;
   esac],
  [custom_fields=false])
AC_MSG_RESULT([${enable_custom-fields}])
AM_CONDITIONAL([CUSTOM_FIELDS], [test "x$custom_fields" = xtrue])


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_CONFIG_SUBDIRS([libcintelhex])
AC_OUTPUT

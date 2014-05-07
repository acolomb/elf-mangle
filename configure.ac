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


AC_INIT([elf-mangle], [0.1], [andre.colomb@auteko.de])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/elf-mangle.c])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC_C99

AC_USE_SYSTEM_EXTENSIONS
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_FUNC_MMAP
AC_CHECK_LIB([elf], [elf_version])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SUBDIRS([libcintelhex])
AC_OUTPUT

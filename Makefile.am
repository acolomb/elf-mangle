# Copyright (C) 2014, 2015  Andre Colomb
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


SUBDIRS = po $(SUBDIRS_INTELHEX) $(SUBDIRS_GNULIB) src

if HAVE_INTELHEX_INTERNAL
SUBDIRS_INTELHEX = libcintelhex
endif

if USE_GNULIB
SUBDIRS_GNULIB = gnulib
endif

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = ./config.rpath
EXTRA_DIST += m4/gnulib-cache.m4
EXTRA_DIST += README.md LICENSE.txt TODO.md

doc_DATA = README.md LICENSE.txt TODO.md ChangeLog

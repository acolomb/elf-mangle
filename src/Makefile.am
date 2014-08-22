# Hey Emacs, this is a -*- makefile -*-

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


AM_CPPFLAGS = -D_POSIX $(CPPFLAGS_INTELHEX_INTERNAL) $(CPPFLAGS_GNULIB)
AM_CFLAGS = -Wall -Wstrict-prototypes -Wextra -fgnu89-inline
AM_LDFLAGS = $(LDFLAGS_INTELHEX_INTERNAL)
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

if HAVE_INTELHEX_INTERNAL
CPPFLAGS_INTELHEX_INTERNAL = -I$(top_srcdir)/libcintelhex/include
LDFLAGS_INTELHEX_INTERNAL = -L$(top_builddir)/libcintelhex/src/.libs -static
endif

if USE_GNULIB
CPPFLAGS_GNULIB = -I$(top_builddir)/gnulib -I$(top_srcdir)/gnulib
endif


noinst_LIBRARIES = libfallback.a
bin_PROGRAMS = elf-mangle
EXTRA_DIST = include_order.txt


libfallback_a_SOURCES =		\
	fallback_options.c	\
	fallback_known_fields.c

elf_mangle_SOURCES =		\
	elf-mangle.c		\
	options.c		\
	options.h		\
	override.c		\
	override.h		\
	print_symbols.c		\
	print_symbols.h		\
	transform.c		\
	transform.h		\
	image_formats.c		\
	image_formats.h		\
	$(IMAGE_IHEX)		\
	image_raw.c		\
	image_raw.h		\
	symbol_map.c		\
	symbol_map.h		\
	symbol_list.c		\
	symbol_list.h		\
	known_fields.h		\
	field_print.c		\
	field_print.h		\
	field_list.c		\
	field_list.h		\
	nvm_field.c		\
	nvm_field.h		\
	find_string.c		\
	find_string.h		\
	intl.h			\
	gettext.h


if HAVE_INTELHEX
IMAGE_IHEX = 			\
	image_ihex.c		\
	image_ihex.h
LIBS_CINTELHEX = -lcintelhex
endif

if CUSTOM_OPTIONS
elf_mangle_SOURCES +=		\
	custom_options.c
endif
if CUSTOM_FIELDS
elf_mangle_SOURCES +=		\
	custom_known_fields.c
endif

if USE_GNULIB
LIBS_GNULIB = $(top_builddir)/gnulib/libgnu.a
endif

elf_mangle_LDADD = $(LIBS_CINTELHEX) $(LIBS_GNULIB) @LIBINTL@
elf_mangle_LDADD += libfallback.a

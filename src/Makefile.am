# Copyright (C) 2014, 2016  Andre Colomb
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


AM_CPPFLAGS = -D_POSIX $(CINTELHEX_INTERNAL_CPPFLAGS) $(GNULIB_CPPFLAGS)
AM_CFLAGS = -Wall -Wstrict-prototypes -Wextra -fgnu89-inline
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

if HAVE_INTELHEX_INTERNAL
CINTELHEX_INTERNAL_CPPFLAGS = -I$(top_srcdir)/libcintelhex/include
CINTELHEX_LIBS += $(top_builddir)/libcintelhex/src/libcintelhex.la
endif

if USE_GNULIB
GNULIB_CPPFLAGS = -I$(top_builddir)/gnulib -I$(top_srcdir)/gnulib
GNULIB_LIBS = $(top_builddir)/gnulib/libgnu.la
endif


noinst_LTLIBRARIES =		\
	libfallback.la		\
	libelf-mangle.la
bin_PROGRAMS = elf-mangle lpstrings
EXTRA_DIST = include_order.txt


libfallback_la_SOURCES =	\
	fallback_options.c	\
	fallback_known_fields.c

libelf_mangle_la_SOURCES =	\
	override.c		\
	override.h		\
	print_symbols.c		\
	print_symbols.h		\
	transform.c		\
	transform.h		\
	image_formats.c		\
	image_formats.h		\
	$(IMAGE_IHEX_INPUT)	\
	image_ihex_output.c	\
	image_ihex.h		\
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
libelf_mangle_la_LIBADD = $(CINTELHEX_LIBS) $(GNULIB_LIBS) $(LTLIBINTL)


if HAVE_INTELHEX
IMAGE_IHEX_INPUT = 		\
	image_ihex_input.c
endif


elf_mangle_SOURCES =		\
	elf-mangle.c		\
	options_elf-mangle.c	\
	options.h

if CUSTOM_OPTIONS
elf_mangle_SOURCES +=		\
	custom_options.c
endif
if CUSTOM_FIELDS
elf_mangle_SOURCES +=		\
	custom_known_fields.c
endif

elf_mangle_LDADD = libelf-mangle.la
elf_mangle_LDADD += libfallback.la


lpstrings_SOURCES =		\
	lpstrings.c		\
	options_lpstrings.c	\
	options.h

lpstrings_LDADD = libelf-mangle.la

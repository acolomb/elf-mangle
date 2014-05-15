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


AM_CPPFLAGS =					\
	-I$(top_builddir)/libcintelhex/include	\
	-I$(top_srcdir)/libcintelhex/include	\
	-D_POSIX
AM_CFLAGS = -Wall -Wstrict-prototypes -Wextra -fgnu89-inline
AM_LDFLAGS = -L$(top_builddir)/libcintelhex/bin -static


noinst_LIBRARIES = libfallback.a
bin_PROGRAMS = elf-mangle


libfallback_a_SOURCES =		\
	fallback.c

dummy_SRC =			\
	dummy_options.c		\
	dummy_known_fields.c

elf_mangle_SOURCES =		\
	elf-mangle.c		\
	options.c		\
	override.c		\
	print_symbols.c		\
	transform.c		\
	image_formats.c		\
	image_ihex.c		\
	image_raw.c		\
	symbol_map.c		\
	symbol_list.c		\
	field_print.c		\
	field_list.c		\
	nvm_field.c		\
	find_string.c		\
	$(dummy_SRC)

elf_mangle_LDADD = -lcintelhex
elf_mangle_LDADD += libfallback.a

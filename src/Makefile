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


SRC = elf-mangle.c		\
	dummy_options.c		\
	options.c		\
	override.c		\
	transform.c		\
	print_symbols.c		\
	nvm_image.c		\
	symbol_map.c		\
	symbol_list.c		\
	dummy_known_fields.c	\
	field_print.c		\
	field_list.c		\
	nvm_field.c

override CFLAGS += -Wall -Wstrict-prototypes -Wextra -Werror -std=c99 -D_POSIX
override LDLIBS += -lelf

elf-mangle: $(SRC)

clean:
	rm -f elf-mangle

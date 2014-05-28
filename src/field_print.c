///@file
///@brief	Pretty-print data based on field descriptions
///@copyright	Copyright (C) 2014  Andre Colomb
///
/// This file is part of elf-mangle.
///
/// This file is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Lesser General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// elf-mangle is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.
///
/// You should have received a copy of the GNU Lesser General Public
/// License along with this program.  If not, see
/// <http://www.gnu.org/licenses/>.
///
///@author	Andre Colomb <andre.colomb@auteko.de>


#include "config.h"

#include "field_print.h"
#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>



void
print_hex_dump(const char *data, size_t size)
{
    static const size_t row_length = 0x10;

    size_t offset;

    // Output header line
    putchar('\t');
    for (offset = 0; offset < row_length; ++offset) {
	printf(" %2zX", offset);
    }
    // Output data
    for (offset = 0; offset < size; ++offset) {
	// Prefix each row with hex offset
	if (offset % row_length == 0) printf("\n\t0x%02zx:\t", offset);
	printf(" %02hhX", (unsigned char) data[offset]);
    }

}



void
print_field(const nvm_field *field,
	    const char *data, size_t size)
{
    if (size < field->expected_size) {
	printf(_("\tMissing %zu of %zu data bytes!"),
	       field->expected_size - size, field->expected_size);
    }
    if (field->print_func) {
	printf("\n\t");
	field->print_func(data, size);
    }
    putchar('\n');
}

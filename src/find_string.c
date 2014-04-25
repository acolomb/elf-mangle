///@file
///@brief	Locate special strings in binary images
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

#include "find_string.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#ifdef DEBUG
#undef DEBUG
#endif



/// Default minimum length of strings to locate
#define MIN_LENGTH_DEFAULT	(4)



const char*
nvm_string_find(const char* blob, size_t size, uint8_t min_length)
{
    const char *c;
    uint8_t printable = 0;

    if (min_length == 0) min_length = MIN_LENGTH_DEFAULT;

    for (c = blob + 1; c < blob + size; ++c) {
#ifdef DEBUG
	printf("[%04zx] ", c - blob);
#endif
	if (*c == '\0') {		//string ends at NUL terminator
#ifdef DEBUG
	    puts("NUL");
#endif
	    while (printable >= min_length) {
#ifdef DEBUG
		printf("\tprintable=%u length=%hhu\n", printable, c[-printable - 1]);
#endif
		if (printable + 1 == (uint8_t) c[-printable - 1]) return c - printable;
		--printable;
	    }
	    printable = 0;
	} else if (isprint(*c)) {	//printable character, might be part of string
	    if (printable < UINT8_MAX) ++printable;
#ifdef DEBUG
	    printf("'%c'", *c);
#endif
	} else {
	    printable = 0;
#ifdef DEBUG
	    printf("<%03hhu>", *c);
#endif
	}
#ifdef DEBUG
	printf("\tprintable=%u\n", printable);
#endif
    }
    return NULL;
}



#ifdef TEST_FIND_STRING
/// Test program with demo function calls
int
main(void)
{
    const char data[] =
	"01234"
	"\x9"
	"12345678"
	"\0"

	"foo"
	"\x20"
	"123456789abcdef "	//0x10
	"123456789abcdef"	//0x1f
	"\0"			//0x20

	"\xFF"
	"123456789abcdef "	//0x10
	"123456789abcdef "	//0x20
	"123456789abcdef "	//0x30
	"123456789abcdef "	//0x40
	"123456789abcdef "	//0x50
	"123456789abcdef "	//0x60
	"123456789abcdef "	//0x70
	"123456789abcdef "	//0x80
	"123456789abcdef "	//0x90
	"123456789abcdef "	//0xa0
	"123456789abcdef "	//0xb0
	"123456789abcdef "	//0xc0
	"123456789abcdef "	//0xd0
	"123456789abcdef "	//0xe0
	"123456789abcdef "	//0xf0
	"123456789abcde"	//0xfe
	"\0"			//0xff
	;
    const char *start = data, *next;
    size_t size = sizeof(data);

    while (size) {
	next = nvm_string_find(start, size, 1);
	if (! next) break;
	printf("String at offset 0x%zx (%zu bytes + NUL):\n\t"
	       "\"%s\"\n", next - start, strlen(next), next);
	next += strlen(next) + 1;
	size -= next - start;
	start = next;
    }

    return 0;
}
#endif

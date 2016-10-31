///@file
///@brief	Locate special strings in binary images
///@copyright	Copyright (C) 2014, 2015  Andre Colomb
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
///@author	Andre Colomb <src@andre.colomb.de>


#include "config.h"

#include "find_string.h"
#include "intl.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

/// Compile diagnostic output messages?
#define DEBUG 0



/// The structure of strings searched consists of:
/// - a single byte whose unsigned value specifies the length
/// - followed by only printable characters (see isprint())
/// - terminated by a NUL character which is included in the
///   byte count for the length field
///
/// Strings not matching this structure or which have less than the
/// specified minimum number of printable characters are ignored.  The
/// returned address points to the first character of the first string
/// found.  To find more strings, call this function again with
/// adjusted start address and size.
const char*
nvm_string_find(const char* blob, size_t size, uint8_t min_length)
{
    const char *c;
    uint8_t printable = 0;

    if (min_length == 0) min_length = FIND_STRING_DEFAULT_LENGTH;

    for (c = blob + 1; c < blob + size; ++c) {	//skip first possible length byte
	if (DEBUG) printf("[%04zx] ", c - blob);
	if (*c == '\0') {		//string ends at NUL terminator
	    if (DEBUG) puts("NUL");
	    while (printable >= min_length) {
		if (DEBUG) printf("\tprintable=%u length=%hhu\n", printable, c[-printable - 1]);
		if (printable + 1			//number of printable characters plus NUL
		    == (uint8_t) c[-printable - 1])	//matches length byte value
		    return c - printable;
		--printable;
	    }
	    printable = 0;
	} else if (isprint((int) *c)) {	//printable character, might be part of string
	    if (printable < UINT8_MAX) ++printable;
	    if (DEBUG) printf("'%c'", *c);
	} else {
	    printable = 0;
	    if (DEBUG) printf("<%03hhu>", *c);
	}
	if (DEBUG) printf("\tprintable=%u\n", printable);
    }
    return NULL;
}



/// Strings are located by repeatedly calling nvm_string_find(),
/// skipping over any previous match.
///
/// If non-NULL, the delim parameter is used to separate the located
/// strings for output.  In that case, the offset may be printed in
/// octal=8, decimal=10 or hex=16 as specified in the output_format
/// parameter.  If negative, the string length is also included.  If
/// no delimiter is specified, a human-readable, verbose default
/// format is used.  A nonzero output_format then avoids translation
/// of the message.
int
nvm_string_list(const char* blob, size_t size,
		uint8_t min_length,
		int output_format, const char *delim)
{
    const char *next, *start = blob, *fmt = NULL;
    int found = 0;

    while (size) {
	next = nvm_string_find(start, size, min_length);
	if (! next) break;
	if (delim) {	//output compatibility mode
	    switch (output_format) {
	    case 8:	fmt = "%7zo "; break;
	    case 10:	fmt = "%7zu "; break;
	    case 16:	fmt = "%7zx "; break;
	    case -8:	fmt = "%7zo+%03zo "; break;
	    case -10:	fmt = "%7zu+%03zu "; break;
	    case -16:	fmt = "%7zx+%03zx "; break;
	    default:	break;
	    }
	    if (fmt) printf(fmt, next - 1 - blob, strlen(next));
	    printf("%s%s", next, delim);
	} else {
	    printf(output_format
		   ? ("lpstring [%04zx] (%zu bytes + NUL):\n\t"
		      "\"%s\"\n")
		   : _("Length prefixed string at offset [%04zx] (%zu bytes + NUL):\n\t"
		       "\"%s\"\n"),
		   next - 1 - blob, strlen(next), next);
	}
	next += strlen(next) + 1;
	size -= next - start;
	start = next;
	found += 1;
    }

    return found;
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

    return nvm_string_list(data, sizeof(data), 0, -16, " EOS\n\n");
}
#endif

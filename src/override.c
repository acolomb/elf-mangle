///@file
///@brief	Override symbol data from key-value string specification
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

#include "override.h"
#include "symbol_list.h"
#include "known_fields.h"
#include "nvm_field.h"
#include "intl.h"

#undef __STRICT_ANSI__	//required on Cygwin for getsubopt()
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

/// Compile diagnostic output messages?
#define DEBUG 0



char*
override_append(char *overrides, const char *append_fmt, ...)
{
    static const char delimiter[] = ",";
    char *new_string;
    size_t old_length, add_length;
    va_list args, args_copy;

    if (! append_fmt) return overrides;

    va_start(args, append_fmt);
    // Need a copy of argument list to find the required length first
    va_copy(args_copy, args);

    // Length of any previous overrides, plus delimiter
    old_length = overrides ? (strlen(overrides) + strlen(delimiter)) : 0;
    // Find length of appended override directive, plus NUL terminator
    add_length = vsnprintf(NULL, 0, append_fmt, args_copy) + sizeof(*new_string);

    if (DEBUG) printf("%s: %zu + %zu\t(%s)\n", __func__, old_length, add_length, append_fmt);

    new_string = realloc(overrides, old_length + add_length);
    if (new_string) {
	// Insert delimiter after any previous override directives
	if (old_length) strcpy(new_string + old_length - strlen(delimiter), delimiter);
	// Overwrite new portion with given formatted output
	if (vsnprintf(new_string + old_length, add_length, append_fmt, args)
	    >= (int) add_length) {
	    fprintf(stderr, _("%s: Override directive truncated during append.\n"), __func__);
	}

	if (DEBUG) printf("%1$s: %2$p -> %3$p (%3$s)\n", __func__, overrides, new_string);
	overrides = new_string;
    } else {
	fprintf(stderr, _("Could not append override directive \""));
	vfprintf(stderr, append_fmt, args);
	fprintf(stderr, _("\".\n"));
    }

    va_end(args_copy);
    va_end(args);

    return overrides;
}



///@brief Parse a string of hexadecimal byte values and write result into buffer
///@details Each byte is specified by two hex digits with arbitrary leading white-space
///@return Number of bytes successfully parsed or negative value on error
static int
parse_hex_bytes(
    const char *start,		///< [in] Zero-terminated string of byte values
    char *output,		///< [out] Buffer to write converted data bytes
    int max_length)		///< [in] Size of output buffer
{
    int parsed = 0, converted;
    unsigned char value;

    if (! start || ! output) return -1;

    while (*start && parsed < max_length) {
	if (1 == sscanf(start, " %2hhx%n", &value, &converted)) {
	    output[parsed++] = value;
	    if (DEBUG) printf("%.*s=%02hhx,\t", converted, start, value);
	    start += converted;
	} else {
	    fprintf(stderr, _("Failed after parsing %d hex bytes with \"%s\" remaining (%s)\n"),
		    parsed, start, strerror(errno));
	    break;
	}
    }
    return parsed;
}



///@brief Mirror token list from known symbols for usage with getsubopt()
///@return Negative on error, number of filled symbol strings otherwise
static inline int
cache_symbols_for_getsubopt(
    const nvm_symbol *list,	///< [in] List of symbols to apply overrides
    const int size,		///< [in] Number of symbols in the list
    const char* symbols[])	///< [out] Cached list of symbols, MUST have (size+1) elements!
{
    int i;

    for (i = 0; i < size; ++i) {
	if (list[i].field) symbols[i] = list[i].field->symbol;
	else {
	    fprintf(stderr, _("%s: Missing symbol name in list member %d\n"), __func__, i);
	    return -2;
	}
    }
    symbols[i] = NULL;	//termination

    return i;
}



/// Apply override specification string to the listed symbols' data
static int
parse_overrides_cached(
    char* restrict overrides,	///< [in] @sa parse_overrides()
    const nvm_symbol* restrict list,	///< [in] @sa parse_overrides()
    const int size,		///< [in] @sa parse_overrides()
    const char* symbols[])	///< [in] Cached list of symbols, NULL-terminated
{
    char *substart, *subopt, *value;
    const char *errmsg;
    int i, parsed = 0, length;

    subopt = overrides;
    while (*subopt != '\0') {
	substart = subopt;
	i = getsubopt(&subopt, (char**) symbols, &value);
	if (i >= 0 && i < size) {
	    length = parse_hex_bytes(value, list[i].blob_address, list[i].size);
	    if (DEBUG) printf(_("Parsed %d bytes for field %s\n"), length, symbols[i]);
	    if (length > 0) {
		++parsed;
		continue;
	    } else errmsg = _("Could not parse byte data");
	} else errmsg = _("Field not found");
	fprintf(stderr, _("Unable to parse override `%.*s' (%s)\n"),
		(int) (subopt - substart), substart , errmsg);
    }
    return parsed;
}



int
parse_overrides(char* restrict overrides, const nvm_symbol* restrict list, const int size)
{
    const char* symbols[size + 1];

    if (! overrides || ! list) return -1;
    if (DEBUG) printf(_("%s: \"%s\"\n"), __func__, overrides);

    if (cache_symbols_for_getsubopt(list, size, symbols) < 0) return -2;

    return parse_overrides_cached(overrides, list, size, symbols);
}



#ifdef TEST_OVERRIDES
int
main(void)
{
#define CONVERT	"BeeF"
    const char hexbytes[] = "4265 65  464F"; //BeeFO
    char *overrides = NULL, content[sizeof(CONVERT)] = { 0 }, buf[sizeof(CONVERT)] = { 0 };
    int parsed;
    nvm_field fields[] = {
	{ .symbol = "a" },
	{ .symbol = "NOTb" },
	{ .symbol = "c" },
	{ .symbol = "d" },
	{ .symbol = "e" },
    };
    nvm_symbol symbols[] = {
	{ 0, sizeof(CONVERT), buf, 0, &fields[0] },
	{ 0, sizeof(CONVERT), buf, 0, &fields[1] },
	{ 0, sizeof(CONVERT), buf, 0, &fields[2] },
	{ 0, sizeof(CONVERT), buf, 0, &fields[3] },
	{ 0, sizeof(CONVERT), buf, 0, &fields[4] },
    };


    overrides = override_append(overrides, NULL);
    if (overrides) puts(overrides);

    overrides = override_append(overrides, "a=ONE,b=2");
    if (overrides) puts(overrides);

    overrides = override_append(overrides, "c=%d", 3);
    if (overrides) puts(overrides);

    parsed = parse_hex_bytes(hexbytes, content, strlen(CONVERT));
    printf("[%s] \"%s\" (%d bytes) %s\n", hexbytes, content, parsed,
	   strcmp(content, CONVERT) ? "FAIL" : "match");

    overrides = override_append(overrides, "d=%d,e=%s", parsed, content);
    if (overrides) puts(overrides);

    parsed = parse_overrides(overrides, symbols, sizeof(symbols) / sizeof(*symbols));
    printf("Parsed %d overrides.\n", parsed);

    free(overrides);
    return 0;
}
#endif

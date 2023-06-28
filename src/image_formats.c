///@file
///@brief	Handling of different binary image formats
///@copyright	Copyright (C) 2014, 2015, 2016  Andre Colomb
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

#include "image_formats.h"
#include "image_ihex.h"
#include "image_raw.h"
#include "intl.h"

#include <stdio.h>

/// Compile diagnostic output messages?
#define DEBUG 0



int
image_memorize_file(const char *filename,
		    const char **blob, size_t *blob_size,
		    enum image_format format)
{
    int status = 0;

    if (! filename || ! blob || ! blob_size) return -1;

    if (format == formatNone ||
	format == formatIntelHex) {
#if HAVE_INTELHEX
	status = image_ihex_memorize_file(filename, blob, blob_size);
	if (status != 0) return status;
	// Retry with raw binary on failure
	else if (format == formatNone) {
	    fprintf(stderr,
		    _("Image file \"%s\" is not in Intel Hex format, trying raw binary.\n"),
		    filename);
	    format = formatRawBinary;
	}
#else // !HAVE_INTELHEX
	format = formatRawBinary;
#endif
    }

    if (format == formatRawBinary) {
	status = image_raw_memorize_file(filename, blob, blob_size);
	return status;
    }

    fprintf(stderr, _("Invalid input image file format.\n"));
    return -2;
}



int
image_merge_file(const char *filename,
		 const nvm_symbol *list, const int list_size,
		 const size_t blob_size,
		 enum image_format format)
{
    int symbols = 0;

    if (! filename || ! blob_size) return -1;

    if (format == formatNone ||
	format == formatIntelHex) {
#if HAVE_INTELHEX
	symbols = image_ihex_merge_file(filename, list, list_size, blob_size);
	if (symbols != 0) return symbols;
	// Retry with raw binary on failure
	else if (format == formatNone) {
	    fprintf(stderr,
		    _("Image file \"%s\" is not in Intel Hex format, trying raw binary.\n"),
		    filename);
	    format = formatRawBinary;
	}
#else // !HAVE_INTELHEX
	format = formatRawBinary;
#endif
    }

    if (format == formatRawBinary) {
	symbols = image_raw_merge_file(filename, list, list_size, blob_size);
	return symbols;
    }

    fprintf(stderr, _("Invalid input image file format.\n"));
    return -2;
}



ssize_t
image_write_file(const char* restrict filename,
		 const char* restrict blob, const size_t blob_size,
		 const enum image_format format)
{
    if (! filename || ! blob || ! blob_size) return -1;

    if (DEBUG) printf(_("%s: Output file \"%s\" format %d\n"), __func__, filename, format);
    switch (format) {
    case formatRawBinary:
	return image_raw_write_file(filename, blob, blob_size);

    case formatIntelHex:
	return image_ihex_write_file(filename, blob, blob_size);

    case formatNone:
    default:
	fprintf(stderr, _("Invalid output image file format specified.\n"));
	return -2;
    }
}

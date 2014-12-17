///@file
///@brief	Handle input of blob data from Intel Hex files
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

#include "image_ihex.h"
#include "image_raw.h"
#include "intl.h"

#include <cintelhex.h>

#include <stdio.h>
#include <errno.h>

#ifdef DEBUG
#undef DEBUG
#endif



int
image_ihex_merge_file(const char *filename,
		      const nvm_symbol *list, int list_size,
		      size_t blob_size)
{
    ihex_recordset_t *rs;
    uint32_t start, end;
    char *blob;
    int symbols = 0;

    if (! filename || ! blob_size) return -1;	//invalid parameters

    rs = ihex_rs_from_file(filename);
    if (! rs) {
	switch (ihex_errno()) {
	case IHEX_ERR_INCORRECT_CHECKSUM:
	case IHEX_ERR_NO_EOF:
	case IHEX_ERR_PARSE_ERROR:
	case IHEX_ERR_WRONG_RECORD_LENGTH:
	case IHEX_ERR_UNKNOWN_RECORD_TYPE:
	    // Parse error, not a well-formed Intel Hex file
	    return 0;

	case IHEX_ERR_NO_INPUT:
	case IHEX_ERR_MMAP_FAILED:
	case IHEX_ERR_READ_FAILED:
	    // File not accessible
	    symbols = -2;
	    break;

	case IHEX_ERR_MALLOC_FAILED:
	default:
	    // System error
	    symbols = -3;
	    break;
	}
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, ihex_error());
	return symbols;
    }

    if (0 != ihex_rs_get_address_range(rs, &start, &end)) {
	fprintf(stderr, _("Could not determine data range in Intel Hex file \"%s\" (%s)\n"),
		filename, ihex_error());
	symbols = -4;
    } else if (rs->ihrs_count == 0 || start >= end) {
	fprintf(stderr, _("Image file \"%s\" is empty\n"), filename);
    } else {
#ifdef DEBUG
	printf("%s: %s contains range 0x%04" PRIx32 " to 0x%04" PRIx32 "\n",
	       __func__, filename, start, end > 0 ? end - 1 : 0);
#endif
	if (blob_size > end) {
	    fprintf(stderr, _("Image file \"%s\" is too small, %zu of %zu bytes missing\n"),
		    filename, blob_size - end, blob_size);
	    blob_size = end;
	}

	// Allocate and initialize memory for needed ihex content
	blob = calloc(1, blob_size);
	if (! blob) {
	    fprintf(stderr, _("Could not copy data from Intel Hex file \"%s\" (%s)\n"),
		    filename, strerror(errno));
	    symbols = -3;
	} else {
	    if (0 != ihex_byte_copy(rs, (void*) blob, blob_size, 0)) {
		fprintf(stderr, _("Could not copy data from Intel Hex file \"%s\" (%s)\n"),
			filename, ihex_error());
		symbols = -4;
	    } else {
		symbols = image_raw_merge_mem(blob, list, list_size, blob_size);
	    }
	    free(blob);
	}
    }
    ihex_rs_free(rs);
    return symbols;
}

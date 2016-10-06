///@file
///@brief	Handle input of blob data from Intel Hex files
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

#include "image_ihex.h"
#include "image_raw.h"
#include "intl.h"

#include <cintelhex.h>

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

/// Compile diagnostic output messages?
#define DEBUG 0



///@brief Open Intel Hex file and determine content size
///@return 1 on success, 0 for unsupported format or negative error code
static int
image_ihex_open_file(
    const char *filename,	///< [in] Input file path to open
    size_t *file_size,		///< [out] Data address at end of content
    ihex_recordset_t **rs)	///< [out] File access handle (open on success)
{
    uint32_t start, end;
    int status = 0;

    if (! filename || ! rs) return -1;	//invalid parameters

    *rs = ihex_rs_from_file(filename);
    if (! *rs) {
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
	    status = -2;
	    break;

	case IHEX_ERR_MALLOC_FAILED:
	default:
	    // System error
	    status = -3;
	    break;
	}
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, ihex_error());
	return status;
    }

    if (0 != ihex_rs_get_address_range(*rs, &start, &end)) {
	fprintf(stderr, _("Could not determine data range in Intel Hex file \"%s\" (%s)\n"),
		filename, ihex_error());
	status = -4;
    } else if ((*rs)->ihrs_count == 0 || start >= end) {
	fprintf(stderr, _("Image file \"%s\" is empty\n"), filename);
	status = -4;
    } else {
	if (DEBUG) printf(_("%s: %s contains range 0x%04" PRIx32 " to 0x%04" PRIx32 "\n"),
			  __func__, filename, start, end > 0 ? end - 1 : 0);
	if (file_size) *file_size = end;
	return 1;
    }
    ihex_rs_free(*rs);

    return status;
}



int
image_ihex_memorize_file(const char *filename,
			 const char **blob, size_t *blob_size)
{
    ihex_recordset_t *rs = NULL;
    int status;
    char *contents = NULL;

    if (! filename || ! blob || ! blob_size) return -1;	//invalid parameters

    status = image_ihex_open_file(filename, blob_size, &rs);
    if (status <= 0) return status;	//file not accessible

    // Allocate and initialize memory for needed ihex content
    contents = calloc(1, *blob_size);
    if (! contents) {
	fprintf(stderr, _("Could not allocate memory for image data: %s\n"),
	 	strerror(errno));
	status = -3;
    } else {
	if (0 != ihex_byte_copy(rs, (void*) contents, *blob_size, 0)) {
	    fprintf(stderr, _("Could not copy data from Intel Hex file \"%s\" (%s)\n"),
		    filename, ihex_error());
	    status = -4;
	}
	*blob = contents;
    }
    ihex_rs_free(rs);

    return status;
}



int
image_ihex_merge_file(const char *filename,
		      const nvm_symbol *list, int list_size,
		      size_t blob_size)
{
    ihex_recordset_t *rs;
    char *blob;
    int symbols = 0;
    size_t file_size = 0;

    if (! filename || ! blob_size) return -1;	//invalid parameters

    symbols = image_ihex_open_file(filename, &file_size, &rs);
    if (symbols <= 0) return symbols;	//file not accessible

    if (blob_size > file_size) {
	fprintf(stderr, _("Image file \"%s\" is too small, %zu of %zu bytes missing\n"),
		filename, blob_size - file_size, blob_size);
	blob_size = file_size;
    }

    // Allocate and initialize memory for needed ihex content
    blob = calloc(1, blob_size);
    if (! blob) {
	fprintf(stderr, _("Could not allocate memory for image data: %s\n"),
		strerror(errno));
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
    ihex_rs_free(rs);

    return symbols;
}

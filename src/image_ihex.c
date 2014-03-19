///@file
///@brief	Handle input and output of blob data to Intel Hex files
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
#include <string.h>
#include <errno.h>

#ifdef DEBUG
//#undef DEBUG
#endif



///@name Intel Hex file format parameters
///@{
#define IMAGE_IHEX_WIDTH	(IHEX_WIDTH_16BIT)
#define IMAGE_IHEX_ENDIANNESS	(IHEX_ORDER_LITTLEENDIAN)
///@}



void
image_ihex_merge_file(const char *filename,
		      const nvm_symbol *list, int list_size,
		      size_t blob_size)
{
    ihex_recordset_t *rs;
    ulong_t rs_size;
    char *blob;

    if (! filename || ! blob_size) return;

    rs = ihex_rs_from_file(filename);
    if (rs == NULL || ihex_errno()) {		//file not opened
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, ihex_error());
    } else {
	rs_size = ihex_rs_get_size(rs);
#ifdef DEBUG
	printf("Data size: %lu\n", rs_size);
#endif
	if (rs_size == 0) {
	    fprintf(stderr, _("Image file \"%s\" is empty\n"), filename);
	} else {
	    if (blob_size > rs_size) {
		fprintf(stderr, _("Image file \"%s\" is too small, %zu of %zu bytes missing\n"),
			filename, blob_size - rs_size, blob_size);
		blob_size = rs_size;
	    }

	    // Allocate memory for needed ihex content
	    blob = malloc(blob_size);
	    if (! blob) {
		fprintf(stderr, _("Could not copy data from Intel Hex file \"%s\" (%s)\n"),
			filename, strerror(errno));
	    } else {
		if (0 != ihex_mem_copy(rs, blob, blob_size,
				       IMAGE_IHEX_WIDTH, IMAGE_IHEX_ENDIANNESS)) {
		    fprintf(stderr, _("Could not copy data from Intel Hex file \"%s\" (%s)\n"),
			    filename, ihex_error());
		} else {
		    image_raw_merge_mem(blob, list, list_size, blob_size);
		}
		free(blob);
	    }
	}
	//FIXME ihex_close(rs);
    }
}


#if 0
void
image_ihex_write_file(const char *filename,
		     const char* blob, size_t blob_size)
{
    int fd;

    if (! filename || ! blob || ! blob_size) return;

    fd = open(filename, O_WRONLY | O_CREAT, 0660);
    if (fd == -1) {		//file not opened
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, strerror(errno));
    } else {
	if (ftruncate(fd, (off_t) blob_size) != 0) {
	    fprintf(stderr, _("Cannot resize image file \"%s\" to %zu bytes (%s)\n"),
		    filename, blob_size, strerror(errno));
	} else {
	    write(fd, blob, blob_size);
	}
	close(fd);
    }
}
#endif

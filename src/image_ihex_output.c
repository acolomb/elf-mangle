///@file
///@brief	Handle output of blob data to Intel Hex files
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
#include "intl.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

#ifdef DEBUG
#undef DEBUG
#endif



///@brief Output a single Intel Hex record to file
///@return Written record length or negative error code
static inline int
ihex_write_single_record(
    FILE* restrict out,		///< [in] Output file stream
    uint8_t reclen,		///< [in] Size of record data in bytes
    uint16_t load_offset,	///< [in] Starting load offset of the data bytes
    uint8_t rectyp,		///< [in] Record type
    const char* restrict data)	///< [in] Information or data for record content
{
    int r;
    uint8_t checksum, i;

    if (! out || (reclen && ! data)) return -1;

    // Write header
    r = fprintf(out, ":%02" PRIX8 "%04" PRIX16 "%02" PRIX8,
		reclen, (unsigned) load_offset, rectyp);
    if (r < 0) return r;
    checksum = reclen
	+ ((load_offset >> 8) & 0xFF)
	+ ((load_offset >> 0) & 0xFF) + rectyp;

    // Write data
    for (i = 0; i < reclen; ++i) {
	r = fprintf(out, "%02hhX", (unsigned char) data[i]);
	if (r < 0) return r;
	checksum += data[i];
    }

    // Write checksum
    checksum = -checksum;
    r = fprintf(out, "%02" PRIX8 "\n", checksum);
    if (r < 0) return r;

    return reclen;
}



///@brief Generate and output records of Intel Hex format for binary data
///@return Number of bytes processed
static ssize_t
ihex_write(
    FILE* restrict out,		///< [in] Output file stream
    const char* blob,		///< [in] Binary data to write
    size_t blob_size)		///< [in] Data size in bytes
{
    static const size_t default_length = 0x20;
    static const size_t segment_length = 0x10000;
    static const uint8_t rec_data = 0x00;
    static const uint8_t rec_eof = 0x01;
    static const uint8_t rec_esa = 0x02;

    size_t reclen;
    uint16_t load_offset = 0;
    uint32_t segment_base = 0;
    char usba[2];
    ssize_t nbytes = 0;

    while (blob_size) {
	// Limit record to default length
	if (blob_size < default_length) reclen = blob_size;
	else reclen = default_length;
	// Limit to current segment
	if (load_offset + reclen > segment_length) reclen = segment_length - load_offset;
#ifdef DEBUG
	printf("%s: Record len=%zu source=%p rest=%zu USBA=%" PRIu32 "\n",
	       __func__, reclen, blob, blob_size, segment_base);
#endif

	// Write record data
	if (reclen) ihex_write_single_record(out, reclen, load_offset, rec_data, blob);
	nbytes += reclen;

	// Check segment size limit
	if (load_offset < segment_length - reclen) {
	    load_offset += reclen;
	} else {
	    // Restart with next segment
	    ++segment_base;
	    load_offset = 0;
	    // Calculate Upper Segment Base Address in big-endian order
	    usba[0] = (char) ((segment_base >> 0) & 0xFF);
	    usba[1] = (char) ((segment_base >> 8) & 0xFF);
	    ihex_write_single_record(out, sizeof(usba) / sizeof(*usba), 0, rec_esa, usba);
	}
	// Advance in source data
	blob += reclen;
	blob_size -= reclen;
    }
    // Write closing end-of-file record
    ihex_write_single_record(out, 0, 0, rec_eof, NULL);

    return nbytes;
}



void
image_ihex_write_file(const char* restrict filename,
		      const char* restrict blob, size_t blob_size)
{
    FILE* restrict out;

    if (! filename || ! blob || ! blob_size) return;

    out = fopen(filename, "w");
    if (! out) {		//file not opened
	fprintf(stderr, _("Cannot open output image \"%s\" (%s)\n"), filename, strerror(errno));
    } else {
	ihex_write(out, blob, blob_size);
	fclose(out);
    }
}

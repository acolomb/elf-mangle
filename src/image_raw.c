///@file
///@brief	Handle input and output of blob data to raw binary files
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
///@author	Andre Colomb <andre.colomb@auteko.de>


#include "config.h"

#include "image_raw.h"
#include "symbol_list.h"
#include "nvm_field.h"
#include "intl.h"

#ifdef HAVE_MMAP
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/// Compile diagnostic output messages?
#define DEBUG 0

// Flags for open() system call
#ifndef O_BINARY
#define O_BINARY	0
#endif

// Default to seek-based implementation
#ifndef HAVE_MMAP
#define HAVE_MMAP 0
#endif



/// Structure of data passed to read iterator function
struct read_symbols_config {
    /// Reference to the source of binary data
    union {
	/// Base address of memory mapped data
	const void*	addr;
	/// File descriptor for file-based data
	int		fd;
    }			source;
    /// Size of source data in bytes
    size_t		size;
};



///@brief Iterator function to read symbol content from memory mapped image
///@see symbol_list_iterator_f
static const nvm_symbol*
read_symbol_mem_iterator(
    const nvm_symbol *symbol,	///< [in] Target symbol to copy data
    const void *arg)		///< [in] Data source configuration
{
    const struct read_symbols_config *conf = arg;
    size_t available;

    // Abort iteration if no data is available
    if (! conf || ! conf->source.addr) return symbol;

    if (symbol->blob_address) {					//destination ok
	// Calculate maximum size of available source data
	if (conf->size > symbol->offset + symbol->size) available = symbol->size;
	else if (conf->size > symbol->offset) available = conf->size - symbol->offset;
	else return symbol;	//no data, skip in count
	memcpy(symbol->blob_address, conf->source.addr + symbol->offset, available);
    }
    return NULL;	//count success
}



///@brief Iterator function to read symbol content from image file
///@see symbol_list_iterator_f
static const nvm_symbol*
read_symbol_seek_iterator(
    const nvm_symbol *symbol,	///< [in] Target symbol to copy data
    const void *arg)		///< [in] Data source configuration
{
    const struct read_symbols_config *conf = arg;
    ssize_t bytes_read = 0;
    size_t rest = symbol->size;

    if (symbol->blob_address) {					//destination ok
	if (symbol->offset == (size_t) lseek(conf->source.fd, symbol->offset, SEEK_SET)) {
	    if (DEBUG) printf(_("%s: copy %zu bytes from file offset %zu to %p\n"),
			      __func__, symbol->size, symbol->offset, symbol->blob_address);
	    while (rest > 0) {
		bytes_read = read(conf->source.fd,
				  symbol->blob_address + (symbol->size - rest), rest);
		if (bytes_read > 0) rest -= bytes_read;
		else break;	//error or end of file
	    }
	    if (rest == 0) return NULL;	//count success
	}
	fprintf(stderr, _("Failed to read %s (%zu bytes) from file offset %zu to %p (%s)\n"),
		symbol->field->symbol, symbol->size, symbol->offset,
		symbol->blob_address, strerror(errno));
	return symbol;	//skip in count
    }
    return NULL;	//count success
}



int
image_raw_merge_mem(const void *blob,
		    const nvm_symbol list[], int list_size,
		    size_t blob_size)
{
    struct read_symbols_config conf = {
	.source.addr	= blob,
	.size		= blob_size,
    };

    return symbol_list_foreach_count(list, list_size, read_symbol_mem_iterator, &conf);
}



int
image_raw_merge_filedes(int fd,
			const nvm_symbol list[], int list_size,
			size_t blob_size)
{
    struct read_symbols_config conf = {
	.source.fd	= fd,
	.size		= blob_size,
    };

    return symbol_list_foreach_count(list, list_size, read_symbol_seek_iterator, &conf);
}



int
image_raw_merge_file(const char *filename,
		     const nvm_symbol list[], int list_size,
		     size_t blob_size)
{
    char *mapped = NULL;
    int fd, symbols = 0;
    struct stat st;

    if (! filename || ! blob_size) return -1;	//invalid parameters

    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd == -1 || 0 != fstat(fd, &st)) {	//file not accessible
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, strerror(errno));
	return -2;
    }

    if (st.st_size == 0) {
	fprintf(stderr, _("Image file \"%s\" is empty\n"), filename);
    } else {
	if (blob_size > (size_t) st.st_size) {
	    fprintf(stderr, _("Image file \"%s\" is too small, %zu of %zu bytes missing\n"),
		    filename, blob_size - (size_t) st.st_size, blob_size);
	    blob_size = st.st_size;
	}

#if HAVE_MMAP
	mapped = mmap(NULL, blob_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED) {
	    mapped = NULL;
	    fprintf(stderr, "%s: mmap() failed (%s)\n", __func__, strerror(errno));
	}
#endif
	if (mapped) {
	    symbols = image_raw_merge_mem(mapped, list, list_size, blob_size);
#if HAVE_MMAP
	    munmap(mapped, blob_size);
#endif
	} else {
	    symbols = image_raw_merge_filedes(fd, list, list_size, blob_size);
	}
    }
    close(fd);

    return symbols;
}



void
image_raw_write_file(const char *filename,
		     const char* blob, size_t blob_size)
{
    int fd;
    ssize_t bytes_written;
    size_t rest = blob_size;

    if (! filename || ! blob || ! blob_size) return;

    fd = open(filename, O_WRONLY | O_CREAT | O_BINARY, 0660);
    if (fd == -1) {		//file not opened
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, strerror(errno));
    } else {
	if (ftruncate(fd, (off_t) blob_size) != 0) {
	    fprintf(stderr, _("Cannot resize image file \"%s\" to %zu bytes (%s)\n"),
		    filename, blob_size, strerror(errno));
	} else {
	    while (rest > 0) {
		bytes_written = write(fd, blob + (blob_size - rest), rest);
		if (bytes_written >= 0) rest -= bytes_written;
		else break;	//error
	    }
	    if (rest != 0) fprintf(stderr, _("Cannot write image file \"%s\" (%s)\n"),
				   filename, strerror(errno));
	}
	close(fd);
    }
}

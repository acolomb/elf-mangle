///@file
///@brief	Apply custom post-processing functions to an image
///@copyright	Copyright (C) 2022, 2023  Andre Colomb
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

#include "post_process.h"
#include "intl.h"

#include <stdio.h>

/// Compile diagnostic output messages?
#define DEBUG 0



///@brief Access list of additional post processors
///@return Vector of post processors implemented by other modules, NULL-terminated
const post_process_f*
get_custom_post_processors(void);



int
post_process_image(const char* blob, const size_t blob_size,
		   const nvm_symbol *list, const int size)
{
    const post_process_f *entry;
    int r, modified = 0;

    if (! list) return -1;
    entry = get_custom_post_processors();
    if (! entry) return -2;

    for (; *entry; ++entry) {
	const post_process_f f = *entry;
	// Call post-processor and accumulate return value
	r = f(blob, blob_size, list, size);
	if (r > 0) modified += r;
    }

    if (DEBUG && modified) printf("%s: modified %d symbols\n", __func__, modified);
    return modified;
}

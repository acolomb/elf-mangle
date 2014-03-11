///@file
///@brief	Helper functions to handle data field descriptors
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

#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#undef DEBUG
#endif



const nvm_field*
find_field(const char *symbol,
	   const nvm_field fields[], size_t num_fields)
{
    const nvm_field *field;
    int comp;

    for (field = fields; field < fields + num_fields; ++field) {
	comp = strcmp(field->symbol, symbol);
#ifdef DEBUG
	printf("%s: (%s; %s) = %d\n", __func__, field->symbol, symbol, comp);
#endif
	if (comp == 0) return field;
	else if (comp > 0) break;
    }
    return NULL;
}



size_t
copy_field_verbatim(const nvm_field *field __attribute__((unused)),
		    char *dst, const char *src,
		    size_t max_size)
{
    memcpy(dst, src, max_size);
    return max_size;
}



size_t
copy_field_noop(const nvm_field *field __attribute__((unused)),
		char *dst __attribute__((unused)), const char *src __attribute__((unused)),
		size_t max_size __attribute__((unused)))
{
    return 0;
}

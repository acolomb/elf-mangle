///@file
///@brief	Copy symbol data between different lists
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

#include "symbol_list.h"
#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#undef DEBUG
#endif



/// Structure of data passed to transfer iterator function
struct transfer_config {
    /// List of symbols with source data
    const nvm_symbol*	list_src;
    /// Number of symbols in source list
    int			num_src;
};



///@brief Iterator function to copy symbol content from source list
///@see symbol_list_iterator_f
static const nvm_symbol*
transfer_field_iterator(
    const nvm_symbol *symbol_dst,	///< [in] Target symbol to copy data
    const void *arg)			///< [in] Data source configuration
{
    const struct transfer_config *conf = arg;
    const nvm_symbol *symbol_src;
    size_t copied __attribute__((unused));	//FIXME
    field_copy_f copy_func = copy_field_verbatim;

    symbol_src = symbol_list_find_field(conf->list_src, conf->num_src, symbol_dst->field);
    if (symbol_src) {
#ifdef DEBUG
	printf(_("%s: Target `%s' (%p) matches source symbol %p\n"), __func__,
	       symbol_dst->field->symbol, symbol_dst, symbol_src);
#endif
	if (symbol_dst->field->copy_func) copy_func = symbol_dst->field->copy_func;
	copied = copy_func(
	    symbol_dst->field,
	    symbol_dst->blob_address, symbol_src->blob_address, symbol_dst->size);
#ifdef DEBUG
	printf(_("%s: %zu of %zu bytes copied\n"),
	       symbol_dst->field->symbol, copied, symbol_dst->size);
#endif
    } else {
	fprintf(stderr, "Target map field %s not found in source.", symbol_dst->field->symbol);
    }
    return NULL;	//continue iterating
}



void
transfer_fields(const nvm_symbol *list_src, int num_src,
		const nvm_symbol *list_dst, int num_dst)
{
    struct transfer_config conf = {
	.list_src	= list_src,
	.num_src	= num_src,
    };

#ifdef DEBUG
    printf("%s: Copy %d symbols in to %d out\n", __func__, num_src, num_dst);
#endif
    symbol_list_foreach(list_dst, num_dst, transfer_field_iterator, &conf);
}

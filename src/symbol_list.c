///@file
///@brief	Track lists of processed symbols
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
///@author	Andre Colomb <src@andre.colomb.de>


#include "config.h"

#include "symbol_list.h"
#include "nvm_field.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



nvm_symbol*
symbol_list_append(nvm_symbol *(list[]), int *size)
{
    nvm_symbol *new_list;

    if (! list || ! size) return NULL;

    new_list = realloc(*list, (*size + 1) * sizeof(nvm_symbol));
    if (new_list) {
	*list = new_list;
	++(*size);
    }
    return new_list;
}



const nvm_symbol*
symbol_list_foreach(const nvm_symbol list[], const int size,
		    const symbol_list_iterator_f func, const void *arg)
{
    const nvm_symbol *sym;

    if (! list || ! size) return NULL;

    for (sym = list; sym < list + size; ++sym) {
	if (func(sym, arg) != NULL) return sym;
    }
    return NULL;
}



int
symbol_list_foreach_count(const nvm_symbol list[], const int size,
			  const symbol_list_iterator_f func, const void *arg)
{
    const nvm_symbol *sym;
    int count = 0;

    if (! list || ! size) return 0;

    for (sym = list; sym < list + size; ++sym) {
	if (func(sym, arg) == NULL) ++count;
    }
    return count;
}



///@brief Iterator function to compare field descriptor
///@see symbol_list_iterator_f
static const nvm_symbol*
find_symbol_iterator_field(
    const nvm_symbol *symbol,	///< [in] Current symbol to process
    const void *arg)		///< [in] Address of field descriptor for match
{
    if (symbol->field == arg) return symbol;
    return NULL;	//continue iterating
}



const nvm_symbol*
symbol_list_find_field(const nvm_symbol list[], int size,
		       const nvm_field *field)
{
    return symbol_list_foreach(list, size, find_symbol_iterator_field, field);
}



///@brief Iterator function to compare symbol name
///@see symbol_list_iterator_f
static const nvm_symbol*
find_symbol_iterator_symbol(
    const nvm_symbol *symbol,	///< [in] Current symbol to process
    const void *arg)		///< [in] Symbol name in field descriptor for match
{
    if (symbol->field && strcmp(symbol->field->symbol, arg) == 0) return symbol;
    return NULL;	//continue iterating
}



const nvm_symbol*
symbol_list_find_symbol(const nvm_symbol list[], int size,
			const char *symbol)
{
    return symbol_list_foreach(list, size, find_symbol_iterator_symbol, symbol);
}

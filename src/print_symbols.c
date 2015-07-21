///@file
///@brief	Pretty-print a dump of symbols that were parsed
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

#include "print_symbols.h"
#include "symbol_list.h"
#include "known_fields.h"
#include "field_print.h"
#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <stdlib.h>

/// Compile diagnostic output messages?
#define DEBUG 0



/// Structure of data passed to iterator function
struct print_symbols_config {
    /// Options for symbol description display
    enum show_field	field;
    /// Options for content display
    enum print_content	content;
};



///@brief Iterator function to print symbol data field description
///@see symbol_list_iterator_f
static const nvm_symbol*
print_symbol_description_iterator(
    const nvm_symbol *symbol,	///< [in] Symbol to process
    const void *arg)		///< [in] Display configuration flags
{
    const enum show_field *conf = arg;

    if (*conf & showAddress) printf("[%04zx]\t", symbol->offset);
    if ((*conf & showSymbol) ||
	(! symbol->field->print_func)) printf("%s:", symbol->field->symbol);
    else printf("%s:", _(symbol->field->description));
    if (*conf & showByteSize) printf(_(" %zu bytes"), symbol->size);
    if (DEBUG) printf(" %p", symbol->blob_address);

    return NULL;	//continue iterating
}



///@brief Iterator function to print symbol content
///@see symbol_list_iterator_f
static const nvm_symbol*
print_symbol_content_iterator(
    const nvm_symbol *symbol,	///< [in] Symbol to process
    const void *arg)		///< [in] Display configuration flags
{
    const enum print_content *conf = arg;
    nvm_field field = *symbol->field;

    switch (*conf) {
    case printHex:	field.print_func = print_hex_dump;	break;
    case printNone:	field.print_func = NULL;		break;
    case printPretty:	break;
    default:		break;
    }
    print_field(&field, symbol->blob_address, symbol->size);

    return NULL;	//continue iterating
}



///@brief Iterator function to print symbol field description and content
///@see symbol_list_iterator_f
static const nvm_symbol*
print_symbol_iterator(
    const nvm_symbol *symbol,	///< [in] Symbol to process
    const void *arg)		///< [in] Display configuration flags
{
    const struct print_symbols_config *conf = arg;

    print_symbol_description_iterator(symbol, &conf->field);
    print_symbol_content_iterator(symbol, &conf->content);

    return NULL;	//continue iterating
}



void
print_symbol_list(const nvm_symbol *list, int size,
		  enum show_field field, enum print_content content)
{
    struct print_symbols_config conf = {
	.field		= field,
	.content	= content,
    };

    if (conf.field || conf.content) {
	symbol_list_foreach(list, size, print_symbol_iterator, &conf);
    }
}

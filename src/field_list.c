///@file
///@brief	Linked list of field descriptors
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

#include "field_list.h"
#include "nvm_field.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/// Compile diagnostic output messages?
#define DEBUG 0



/// Element in the linked list
struct nvm_field_list_entry {
    /// Immutable field descriptor
    const nvm_field	field;
    /// Link to next list elemnt
    field_list_entry*	next;
};



const nvm_field*
field_list_find(const char *symbol, const nvm_field_list *list)
{
    field_list_entry *entry;
    int comp;

    if (! list) return NULL;

    for (entry = list->start; entry; entry = entry->next) {
	comp = strcmp(entry->field.symbol, symbol);
	if (DEBUG) printf("%s: (%s; %s) = %d\n", __func__, entry->field.symbol, symbol, comp);
	if (comp == 0) return &entry->field;
	else if (comp > 0) break;
    }
    return NULL;
}



const nvm_field*
field_list_add(nvm_field_list *list,
	       size_t expected_size, const char *symbol,
	       const char *description)
{
    nvm_field *field;
    field_list_entry *entry;

    if (! list) return NULL;

    entry = calloc(1, sizeof(*entry));
    if (entry) {
	// Initialize new element through non-const pointer
	field = (nvm_field*) &entry->field;
	field->expected_size = expected_size;
	field->symbol = symbol;
	field->description = description;
	// Insert new field entry at given tail position
	if (list->tail) {
	    entry->next = list->tail->next;
	    list->tail->next = entry;
	}
	// Adjust new chain tail position
	list->tail = entry;
	// Adjust chain start if there was previously no element
	if (! list->start) list->start = entry;
	return &entry->field;
    }
    return NULL;
}

///@file
///@brief	Growable list of field descriptors with invariant addresses
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


#ifndef FIELD_LIST_H_
#define FIELD_LIST_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_field nvm_field;


/// Opaque type for a list entry
typedef struct nvm_field_list_entry field_list_entry;

/// List of field descriptors
typedef struct nvm_field_list {
    /// Access to the first list entry
    field_list_entry*	start;
    /// Direct access to the last list entry
    field_list_entry*	tail;
} nvm_field_list;


///@brief Find the field descriptor matching a symbol name
///@return Address of the field descriptor or NULL on error
const nvm_field* field_list_find(
    const char *symbol,		///< [in] Symbol name to look for
    const nvm_field_list *list	///< [in] Field descriptor list
    );

///@brief Append and initialize a new field descriptor to the list
///@invariant References to any previously added field descriptors remain valid
///@return Reference to the newly added field descriptor
const nvm_field* field_list_add(
    nvm_field_list *list,	///< [in,out] Field descriptor list
    size_t expected_size,	///< [in] Expected symbol size to record in field descriptor
    const char *symbol,		///< [in] Symbol name to record in field descriptor
    const char *description	///< [in] Intelligible field name to record in field descriptor
    );

#endif //FIELD_LIST_H_

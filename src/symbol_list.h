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
///@author	Andre Colomb <andre.colomb@auteko.de>


#ifndef SYMBOL_LIST_H_
#define SYMBOL_LIST_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_field nvm_field;


/// Description of a meaningful location within binary data
typedef struct nvm_symbol {
    /// Position of the data within blobs, according to symbol map
    size_t		offset;
    /// Size of the data field in bytes
    size_t		size;
    /// Storage address of the current value in memory
    char*		blob_address;
    /// Reference to a field descriptor to identify the type of data
    const nvm_field*	field;
} nvm_symbol;

///@brief Function pointer to iterate through a list of symbols
///@return Interpreted differently by symbol_list_foreach() / symbol_list_foreach_count():
/// - NULL to continue iteration / increase success count
/// - Result found to end the iteration / skip in count
typedef const nvm_symbol* (*symbol_list_iterator_f)(
    const nvm_symbol *symbol,		///< [in] Current symbol to process
    const void *arg			///< [in] Custom data to control iteration
    );


///@brief Append an element to the list
///@note All references to symbols in the list become invalid on success
///@return New location of the first symbol in the list or NULL on error
nvm_symbol* symbol_list_append(
    nvm_symbol *(list[]),		///< [in,out] Start location of the list
    int *size				///< [out] New list size
    );

///@brief Iterate through a list of symbols and call the given function
///@details Iteration stops after the iterator function returns a non-NULL value
///@return Return value of the last iterator function call
const nvm_symbol* symbol_list_foreach(
    const nvm_symbol list[],		///< [in] List of symbols to iterate through
    int size,				///< [in] Number of symbols in the list
    const symbol_list_iterator_f func,	///< [in] Iterator function to be called
    const void *arg			///< [in] Custom data passed to iterator function
    );

///@brief Iterate through a list of symbols and call the given function
///@return Number of iterations where the function returned a NULL value.
int symbol_list_foreach_count(
    const nvm_symbol list[],		///< [in] List of symbols to iterate through
    int size,				///< [in] Number of symbols in the list
    const symbol_list_iterator_f func,	///< [in] Iterator function to be called
    const void *arg			///< [in] Custom data passed to iterator function
    );

///@brief Find the first symbol with a given field descriptor
///@return Address of the first matching symbol or NULL on error
const nvm_symbol* symbol_list_find_field(
    const nvm_symbol list[],		///< [in] List of symbols to search through
    int size,				///< [in] Number of symbols in the list
    const nvm_field *field		///< [in] Field descriptor to look for
    );

///@brief Find the first symbol whose field descriptor matches a given name
///@return Address of the first matching symbol or NULL on error
const nvm_symbol* symbol_list_find_symbol(
    const nvm_symbol list[],		///< [in] List of symbols to search through
    int size,				///< [in] Number of symbols in the list
    const char *symbol			///< [in] Symbol name to look for
    );

#endif //SYMBOL_LIST_H_

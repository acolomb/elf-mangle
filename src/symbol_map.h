///@file
///@brief	Parsing of symbol maps with binary data
///@copyright	Copyright (C) 2014, 2023  Andre Colomb
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


#ifndef SYMBOL_MAP_H_
#define SYMBOL_MAP_H_

#include <stddef.h>

// Forward declarations
typedef struct nvm_symbol nvm_symbol;


/// Opaque type to keep state of symbol map source internals
typedef struct nvm_symbol_map_source nvm_symbol_map_source;


///@brief Open the given file as symbol map
///@return Handle of this source for further processing
nvm_symbol_map_source* symbol_map_open_file(
    const char *filename		///< [in] File path to open
);

///@brief Examine symbol map contents, store symbol list and binary data
///@return
/// - Number of symbols parsed successfully
/// - Zero if no symbols were found
/// - Negative value on error (*symbol_list will be unaltered or NULL)
int symbol_map_parse(
    nvm_symbol_map_source *source,	///< [in,out] Handle of the map source
    const char *section_name,		///< [in] Section within the source to parse
    nvm_symbol **symbol_list,		///< [out] List of symbols found
    int save_values			///< [in] Need separate copies of the original values?
);

///@brief Access the source's binary data
///@return Address of the binary data or NULL on error
char* symbol_map_blob_address(
    const nvm_symbol_map_source *source	///< [in,out] Handle of the map source
);

///@brief Check the size of the source's binary data
///@return Size of binary data or zero on error
size_t symbol_map_blob_size(
    const nvm_symbol_map_source *source	///< [in,out] Handle of the map source
);

///@brief Print out the size of the source's binary data
void symbol_map_print_size(
    const nvm_symbol_map_source *source,///< [in] Handle of the map source
    int parseable			///< [in] Avoid localized output
);

///@brief Close the symbol map source handle and free associated resources
void symbol_map_close(
    nvm_symbol_map_source *source	///< [in,out] Handle of the map source
);

#endif //SYMBOL_MAP_H_

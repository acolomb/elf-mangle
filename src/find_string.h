///@file
///@brief	Locate special strings in binary images
///@copyright	Copyright (C) 2014, 2016  Andre Colomb
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


#ifndef FIND_STRING_H_
#define FIND_STRING_H_

#include <stddef.h>
#include <stdint.h>


/// Default minimum length of strings to locate
#define FIND_STRING_DEFAULT_LENGTH	4


///@brief Find strings prefixed with single-byte length specification
///@return Start of string or NULL if none found
const char* nvm_string_find(
    const char* blob,		///< [in] Binary data to search in
    size_t size,		///< [in] Size of binary data
    uint8_t min_length		///< [in] Minimum length of string to find (zero for default=4)
);

///@brief Find and print all suitable strings in given binary data
///@return Number of strings found or negative error code
int nvm_string_list(
    const char* blob,		///< [in] Binary data to search in
    size_t size,		///< [in] Size of binary data
    uint8_t min_length,		///< [in] Minimum string length passed to nvm_string_find()
    int output_format,		///< [in] Output format configuration
    const char *delim		///< [in] Delimiter for string output, forces simple format if set
);

#endif //FIND_STRING_H_

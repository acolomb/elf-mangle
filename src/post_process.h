///@file
///@brief	Generic interface to apply post-processing functions to an image
///@copyright	Copyright (C) 2022  Andre Colomb
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


#ifndef POST_PROCESS_H_
#define POST_PROCESS_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_symbol nvm_symbol;


///@brief Function pointer to apply image post-processing
///@return Number of symbols modified or negative error code
typedef int (*post_process_f)(
    const char* blob,			///< [in] Binary data to process
    size_t blob_size,			///< [in] Size of binary data
    const nvm_symbol *list,		///< [in] List of symbols to apply modifications
    int size				///< [in] Number of symbols in the list
);

///@brief Apply all available post-processor functions
///@return Number of symbols modified or negative error code (only from this function)
int post_process_image(
    const char* blob,			///< [in] Binary data to process
    size_t blob_size,			///< [in] Size of binary data
    const nvm_symbol *list,		///< [in] List of symbols to apply overrides
    int size				///< [in] Number of symbols in the list
);

#endif //POST_PROCESS_H_

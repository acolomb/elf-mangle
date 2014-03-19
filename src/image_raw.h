///@file
///@brief	Handle input and output of blob data to raw binary files
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


#ifndef IMAGE_RAW_H_
#define IMAGE_RAW_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_symbol nvm_symbol;


///@brief Update each listed symbol's content from memory region
///@return Number of symbols successfully read or negative error code
int image_raw_merge_mem(
    const void *blob,		///< [in] Source memory image
    const nvm_symbol *list,	///< [in] Symbol list start address
    int list_size,		///< [in] Number of symbols in list
    size_t blob_size		///< [in] Data size in the memory image
    );

///@brief Update each listed symbol's content from open binary file
///@return Number of symbols successfully read or negative error code
int image_raw_merge_filedes(
    int fd,			///< [in] Source file descriptor
    const nvm_symbol *list,	///< [in] Symbol list start address
    int list_size,		///< [in] Number of symbols in list
    size_t blob_size		///< [in] Data size in the memory image
    );

///@brief Open image file and update each listed symbol's content
///@return Number of symbols successfully read or negative error code
int image_raw_merge_file(
    const char *filename,	///< [in] Input file path to open
    const nvm_symbol *list,	///< [in] Symbol list start address
    int list_size,		///< [in] Number of symbols in list
    size_t blob_size		///< [in] Expected data size in the image
    );

///@brief Write blob data to image file
void image_raw_write_file(
    const char *filename,	///< [in] Output file path to open
    const char* blob,		///< [in] Binary data to write
    size_t blob_size		///< [in] Data size in bytes
);

#endif //IMAGE_RAW_H_

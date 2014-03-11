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


#ifndef NVM_IMAGE_H_
#define NVM_IMAGE_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_symbol nvm_symbol;


///@brief Open image file and update each listed symbol's content
void nvm_image_merge_file(
    const char *filename,	///< [in] Input file path to open
    const nvm_symbol *list,	///< [in] Symbol list start address
    int list_size,		///< [in] Number of symbols in list
    size_t blob_size		///< [in] Expected data size in the image
    );

///@brief Write blob data to image file
void nvm_image_write_file(
    const char *filename,	///< [in] Output file path to open
    const char* blob,		///< [in] Binary data to write
    size_t blob_size		///< [in] Data size in bytes
);

#endif //NVM_IMAGE_H_

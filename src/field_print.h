///@file
///@brief	Pretty-print data based on field descriptions
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


#ifndef FIELD_PRINT_H_
#define FIELD_PRINT_H_

#include <stddef.h>


// Forward declarations
typedef struct nvm_field nvm_field;


///@brief Output data block as a hexadecimal table dump
void print_hex_dump(
    const char *data,		///< [in] Start address of data block
    size_t size			///< [in] Size of data block in bytes
    );

///@brief Pretty-print data block according to field description
void print_field(
    const nvm_field *field,	///< [in] Field descriptor
    const char *data,		///< [in] Start address of data block
    size_t size			///< [in] Actual size of data block in bytes
    );

#endif //FIELD_PRINT_H_
///@file
///@brief	Required functions to implement intelligent handling of known data fields
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


#ifndef KNOWN_FIELDS_H_
#define KNOWN_FIELDS_H_


// Forward declarations
struct nvm_field;


///@brief Estimate number of fields to be parsed
///@return Number of known field descriptors
int known_fields_expected(void);

///@brief Find field descriptor for a known symbol
///@return Reference to field descriptor or NULL on error
const struct nvm_field* find_known_field(
    const char *symbol		///< [in] Symbol name
    );

#endif //KNOWN_FIELDS_H_

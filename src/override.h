///@file
///@brief	Override symbol data from key-value string specification
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


#ifndef OVERRIDE_H_
#define OVERRIDE_H_


// Forward declarations
typedef struct nvm_symbol nvm_symbol;

///@brief Append to the override specification with delimiter if necessary
///@note The string must be stored on the heap and may be reallocated to a new address
///@return New location of the override specification string
char* override_append(
    char *overrides,		///< [in] The currently allocated override string
    const char *append_fmt,	///< [in] Format string for printf() with additional parameters
    ...) __attribute__((format (printf, 2, 3)));

///@brief Apply override specification string to the listed symbols' data
///@return Number of overrides successfully parsed or negative number for parameter error
int parse_overrides(
    char *overrides,		///< [in] Override specification string
    const nvm_symbol *list,	///< [in] List of symbols to apply overrides
    int size			///< [in] Number of symbols in the list
    );

#endif //OVERRIDE_H_

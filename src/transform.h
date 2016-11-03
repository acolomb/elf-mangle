///@file
///@brief	Copy symbol data between different lists
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


#ifndef TRANSFORM_H_
#define TRANSFORM_H_


// Forward declaration
typedef struct nvm_symbol nvm_symbol;


///@brief Copy data for each symbol in destination list from corresponding symbol in source list
///@details Symbols not found in the source list will not be modified
void transfer_fields(
    const nvm_symbol *list_src,	///< [in] Source list of symbols to copy from
    int num_src,		///< [in] Number of symbols in source list
    const nvm_symbol *list_dst,	///< [in] Destination list of symbols to copy to
    int num_dst			///< [in] Number of symbols in destination list
);

#endif //TRANSFORM_H_

///@file
///@brief	Pretty-print a dump of symbols that were parsed
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


#ifndef PRINT_SYMBOLS_H_
#define PRINT_SYMBOLS_H_


// Forward declaration
typedef struct nvm_symbol nvm_symbol;


/// Options controlling the display of fields
enum show_field {
    showNone		= 0,	///< No output
    showAny		= 1,	///< Dummy value to distinguish 'no flags' from showNone
    showDescription	= 0,	///< Default is to show description
    showSymbol		= 2,	///< Show symbol name instead of field description
    showByteSize	= 4,	///< Add symbol size in bytes
    showAddress		= 8,	///< Add symbol address within blob
    /// Default configuration
    showDefault		= showAny | showDescription,
    /// All available information
    showAll		= showAny | showByteSize | showAddress,
    /// Most interesting for programmers
    showDump		= showAny | showSymbol | showByteSize | showAddress,
};

/// Options controlling the display of interpreted contents
enum print_content {
    printNone		= 0,	///< No output
    printPretty		= 1,	///< Field-specific interpretation
    printHex		= 2,	///< Generic hex dump of byte stream
};

///@brief Print out the listed symbols according to configuration flags
void print_symbol_list(
    const nvm_symbol *list,	///< [in] Symbol list start address
    int size,			///< [in] Number of symbols in list
    enum show_field field,	///< [in] Description display options
    enum print_content content	///< [in] Content display options
    );

#endif //PRINT_SYMBOLS_H_

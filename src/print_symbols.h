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
///@author	Andre Colomb <src@andre.colomb.de>


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
    showSymbolDefine	= 16,	///< One-line "symbol_name=" prefix, machine-parseable
    showByteSize	= 4,	///< Add symbol size in bytes
    showAddress		= 8,	///< Add symbol address within blob
    showFilterChanged	= 32,	///< Show only fields differing from input map
    /// Default configuration
    showDefault		= showAny | showDescription,
    /// All available information
    showAll		= showAny | showByteSize | showAddress,
    /// Most interesting for programmers
    showDump		= showAny | showSymbol | showByteSize | showAddress,
    /// Single-line machine parseable
    showDefines		= showAny | showSymbolDefine,
    /// Only differing symbols in single-line machine parseable format
    showDefinesDiff	= showDefines | showFilterChanged,
};

/// Options controlling the display of interpreted contents
enum print_content {
    printNone		= 0,	///< No output
    printPretty		= 1,	///< Field-specific interpretation, fallback to hex dump
    printPrettyOnly	= 2,	///< Field-specific interpretation, no fallback
    printHex		= 3,	///< Generic hex dump of byte stream
    printDefines	= 4,	///< One-line hex dump, machine-parseable
};

///@brief Print out the listed symbols according to configuration flags
void print_symbol_list(
    const nvm_symbol *list,	///< [in] Symbol list start address
    int size,			///< [in] Number of symbols in list
    enum show_field field,	///< [in] Description display options
    enum print_content content	///< [in] Content display options
);

#endif //PRINT_SYMBOLS_H_

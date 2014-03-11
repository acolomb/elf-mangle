///@file
///@brief	Command line parsing
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


#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "print_symbols.h"


/// Application options
typedef struct tool_config {
    /// Names of input and output map files
    const char*		map_files[2];
    /// ELF section name to examine
    const char*		section;
    /// Name of the input image file
    const char*		image_in;
    /// Name of the output image file
    const char*		image_out;
    /// Configuration flags for dumping symbol descriptions
    enum show_field	show_fields;
    /// Configuration flags for dumping symbol content
    enum print_content	print_content;
    /// Override specification, comma-separated "field=<hexbytes>" pairs
    char*		overrides;	///<@note Must be a heap address valid for free()
} tool_config;


///@brief Parse command line argument vector
///@return Zero on success or error code
int check_opts(
    int argc,			///< [in] Number of tokens on the command line
    char **argv,		///< [in,out] Vector of command line tokens
    tool_config *config		///< [out] Storage for parsed application configuration
    );

#endif //OPTIONS_H_

///@file
///@brief	Compile-time configuration constants
///@copyright	Copyright (C) 2014  Andre Colomb
///
/// This file is part of elf-mangle, a tool to analyze, transform and
/// manipulate binary data based on ELF symbol tables.
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


#include "config.h"

#include "options.h"
#include "override.h"
#include "transform.h"
#include "nvm_image.h"
#include "symbol_map.h"
#include "symbol_list.h"
#include "known_fields.h"
#include "field_print.h"
#include "nvm_field.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/// Default ELF section to use
#define DEFAULT_SECTION		".eeprom"



///@brief Process symbol maps and binary data according to application arguments
static void
process_maps(const tool_config *config)
{
    nvm_symbol_map_source *map_in = NULL, *map_out = NULL, *map_write = NULL;
    nvm_symbol *symbols_in = NULL, *symbols_out = NULL;
    int num_in, num_out;

    // Read input symbol layout and associated image data
    map_in = symbol_map_open_file(config->map_files[0]);
    num_in = symbol_map_parse(map_in, config->section, &symbols_in);
    if ((symbols_in && num_in > 0)		//input map loaded
	&& (! config->image_in			//no input image
	    || 0 < nvm_image_merge_file(	//input image loaded
		config->image_in, symbols_in, num_in,
		symbol_map_blob_size(map_in)))) {
	// Translate data from input to output layout if supplied
	map_out = symbol_map_open_file(config->map_files[1]);
	num_out = symbol_map_parse(map_out, config->section, &symbols_out);
	if (symbols_out && num_out > 0) {
	    map_write = map_out;
	    transfer_fields(symbols_in, num_in, symbols_out, num_out);
	} else {	//no valid output map, use same as input
	    map_write = map_in;
	    symbols_out = symbols_in;
	    num_out = num_in;
	}
	// Incorporate symbol overrides
	parse_overrides(config->overrides, symbols_out, num_out);	//FIXME
	// Print out information if requested
	print_symbol_list(symbols_out, num_out,
			  config->show_fields, config->print_content);
	// Store output image to file
	if (config->image_out) nvm_image_write_file(
	    config->image_out, symbol_map_blob_address(map_write),
	    symbol_map_blob_size(map_write));
    }

    free(symbols_in);
    if (symbols_in != symbols_out) free(symbols_out);

    symbol_map_close(map_in);
    symbol_map_close(map_out);
}



#ifndef TEST_MAIN
int
main(int argc, char **argv)
{
    int ret_code = 0;
    tool_config config = {
	.section		= DEFAULT_SECTION,
	.print_content		= showDefault,
    };

    // Initialize message translation
//FIXME  setlocale (LC_ALL, "");
//FIXME  textdomain (PACKAGE_TARNAME);

    // Parse command line options
    ret_code = check_opts(argc, argv, &config) != 0;
    if (ret_code != 0) return ret_code;//FIXME

    // Process specified actions
    process_maps(&config);

    free(config.overrides);

    return ret_code;
}



#else //TEST_MAIN

/// Test program with demo function calls
int
main(void)
{
    char version_data[] = "\0foo";
    char throttle_data[] = { 0xcd, 0x00, 0x32, 0x03, 0xff, 0x01 };
    char throttle_curve[200] = { 0 };
    nvm_symbol test[] = {
	{ .blob_address	= version_data,
	  .size		= sizeof(version_data), },
	{ .blob_address	= throttle_data,
	  .size		= sizeof(throttle_data), },
	{ .blob_address	= throttle_curve,
	  .size		= sizeof(throttle_curve), },
    };

#if 0
    printf("\n\t");
    print_hex_dump(version_data, sizeof(version_data));
    printf("\n\t");
    print_hex_dump(throttle_curve, sizeof(throttle_curve));
    printf("\n\t");
#endif

    version_data[0] = sizeof(version_data) - 1;
//    version_data[sizeof(version_data) - 1] = 'X';
    printf("data size: %zu length: %u strlen: %zu\n",
	   sizeof(version_data), *version_data, strlen(version_data + 1));

    test[0].field = find_known_field("nvm_version");
    if (test[0].field) {
	printf("%s:", test[0].field->description);
	print_field(test[0].field, test[0].blob_address, test[0].size);
    }


    test[1].field = find_known_field("nvm_throttle_config");
    if (test[1].field) {
	printf("%s:", test[1].field->description);
	print_field(test[1].field, test[1].blob_address, test[1].size);
    }


    test[2].field = find_known_field("nvm_throttle_curve");
    if (test[2].field) {
	printf("%s:", test[2].field->description);
	print_field(test[2].field, test[2].blob_address, test[2].size);
    }
}
#endif

///@file
///@brief	Main program logic
///@copyright	Copyright (C) 2014, 2015, 2016, 2022, 2023  Andre Colomb
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
///@author	Andre Colomb <src@andre.colomb.de>


#include "config.h"

#include "options.h"
#include "post_process.h"
#include "override.h"
#include "transform.h"
#include "symbol_map.h"
#include "symbol_list.h"
#include "known_fields.h"
#include "field_print.h"
#include "nvm_field.h"
#include "gettext.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>



/// Carry out requested actions on final layout according to application arguments
static inline int
process_final_map(const tool_config* restrict config,
		  const nvm_symbol_map_source* restrict map,
		  const nvm_symbol* restrict symbols,
		  const int num)
{
    int r;

    // Incorporate symbol overrides from file
    r = config->overrides_file ? parse_override_file(config->overrides_file, symbols, num) : 0;
    if (r < 0) return r;

    // Incorporate other symbol overrides
    r = config->overrides ? parse_overrides(config->overrides, symbols, num) : 0;
    if (r < 0) return r;

    // Let any custom post-processors scan and manipulate the blob content
    r = post_process_image(symbol_map_blob_address(map), symbol_map_blob_size(map),
			   symbols, num);
    if (r < 0) return r;

    // Print out information if requested
    if (config->show_size) symbol_map_print_size(map, config->show_fields & showSymbol);
    print_symbol_list(symbols, num, config->show_fields, config->print_content);

    // Store output image to file
    if (config->image_out) r = image_write_file(
	config->image_out, symbol_map_blob_address(map), symbol_map_blob_size(map),
	config->format_out);
    if (r < 0) return r;

    return 0;
}



/// Adjust for output layout according to application arguments
static inline int
process_output_map(const tool_config* restrict config,
		   const nvm_symbol_map_source* restrict map_in,
		   const int num_in,
		   const nvm_symbol* restrict symbols_in)
{
    nvm_symbol_map_source *map_out = NULL;
    nvm_symbol *symbols_out = NULL;
    int num_out, ret_code;

    if (! config->map_files[1]) {
	// No valid output map, use same as input
	return process_final_map(config, map_in, symbols_in, num_in);
    }

    // Translate data from input to output layout if supplied
    map_out = symbol_map_open_file(config->map_files[1]);
    num_out = symbol_map_parse(map_out, config->section, &symbols_out,
			       config->show_fields & showFilterChanged);
    if (num_out < 0) return num_out;	//propagate error code

    if (symbols_out) {
	transfer_fields(symbols_in, num_in, symbols_out, num_out);
	ret_code = process_final_map(config, map_out, symbols_out, num_out);
    }

    symbol_list_free(symbols_out, num_out);
    free(symbols_out);

    return ret_code;
}



/// Read and examine blob data from input image according to application arguments
static inline int
process_input_image(const tool_config* restrict config,
		    const nvm_symbol_map_source* restrict map_in,
		    const nvm_symbol* restrict symbols_in,
		    const int num_in)
{
    int ret_code;

    if (config->image_in) {
	ret_code = image_merge_file(config->image_in, symbols_in, num_in,
				    symbol_map_blob_size(map_in), config->format_in);
	if (ret_code < 0) return ret_code;
    }

    // Scan for strings if requested (no error potential)
    if (config->lpstring_min >= 0) nvm_string_list(
	symbol_map_blob_address(map_in), symbol_map_blob_size(map_in),
	config->lpstring_min, config->show_fields & showSymbol,
	NULL);

    ret_code = process_output_map(config, map_in, num_in, symbols_in);

    return ret_code;
}



///@brief Process symbol maps and binary data according to application arguments
///@return Zero for success or no symbols, negative on error
static int
process_maps(const tool_config *config)
{
    nvm_symbol_map_source *map_in = NULL;
    nvm_symbol *symbols_in = NULL;
    int num_in, ret_code;

    // Read input symbol layout and associated image data
    map_in = symbol_map_open_file(config->map_files[0]);
    num_in = symbol_map_parse(map_in, config->section, &symbols_in,
			      config->show_fields & showFilterChanged);
    if (num_in <= 0) ret_code = num_in;	//propagate error code or no symbols
    else ret_code = process_input_image(config, map_in, symbols_in, num_in);

    symbol_list_free(symbols_in, num_in);
    free(symbols_in);
    symbol_map_close(map_in);

    return ret_code;
}



#ifndef TEST_MAIN
int
main(int argc, char **argv)
{
    int ret_code;
    tool_config config = {
	.section		= DEFAULT_SECTION,
	.lpstring_min		= -1,
	.show_size		= 0,
	.show_fields		= showNone,
	.print_content		= printNone,
	.format_out		= formatIntelHex,
    };

    // Initialize message translation
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    // Parse command line options
    ret_code = check_opts(argc, argv, &config) != 0;
    if (ret_code != 0) return ret_code;

    // Process specified actions
    ret_code = -process_maps(&config);

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
    printf("data size: %zu length: %" PRIu8 " strlen: %zu\n",
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

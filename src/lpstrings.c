///@file
///@brief	Main program logic for lpstrings utility
///@copyright	Copyright (C) 2016  Andre Colomb
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


#include "config.h"

#include "options.h"
#include "gettext.h"

#include <locale.h>



///@brief Process binary data according to application arguments
///@return Zero for success, negative on error
static int
process_image(const tool_config *config)
{
    int status, ret_code = 0;
    const char *blob_address;
    size_t blob_size;

    // Read input symbol layout and associated image data
    status = image_memorize_file(	//input image loaded
	config->image_in, &blob_address, &blob_size, config->format_in);
    if (0 < status) {
	// Scan for strings
	nvm_string_list(
	    blob_address, blob_size,
	    config->locate_strings, 0, NULL);
    } else {
	ret_code = status;	//propagate error code
    }

    return ret_code;
}



int
main(int argc, char **argv)
{
    int ret_code;
    tool_config config = {
	.locate_strings		= 0,
	.show_size		= 0,
	.show_fields		= showNone,
	.print_content		= printNone,
    };

    // Initialize message translation
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    // Parse command line options
    ret_code = check_opts(argc, argv, &config) != 0;
    if (ret_code != 0) return ret_code;

    // Process specified actions
    ret_code = -process_image(&config);

    return ret_code;
}

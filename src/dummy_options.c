///@file
///@brief	Command line parsing specific to example system EEPROM images
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


#include "config.h"

#include "options.h"
#include "intl.h"

#include <unistd.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>



/// Offset to distinguish example-specific options from ASCII characters
#define DUMMY_OPT_BASE		(1000)

///@name Option keys
///@{
#define OPT_SET_SERIAL		(DUMMY_OPT_BASE + 1)
///@}


/// Supported command-line arguments definition
static const struct argp_option dummy_options[] =
{
    { "set-serial",	OPT_SET_SERIAL,		N_("BYTES"),		0,
      N_("Override system serial number in output"),			0 },
    { 0 }
};



/// Helper to byteswap one word
static inline int
swap_endian(int input)
{
    int output;
#ifndef _XOPEN_SOURCE
    char *in_bytes = (void*) &input, *out_bytes = (void*) &output;
    out_bytes[0] = in_bytes[1];
    out_bytes[1] = in_bytes[0];
#else
    swab(&input, &output, sizeof(output));
#endif

    return output;
}



///@brief Argp Parser Function for example-specific command line options
///@return Zero or error code specifying how to continue
static error_t
dummy_parse_opt(
    const int key,		///< [in] Key field from the options vector
    char *arg,			///< [in,out] Value given as option argument
    struct argp_state *state)	///< [in,out] Parsing state for use by parser
{
/// Override directive template appended by OPT_SET_SERIAL
#define SET_SERIAL_OVERRIDE	"nvm_unique=%04x"

    // Retreive the input argument from argp_parse
    struct tool_config *tool = state->input;
    char *overrides = tool->overrides;

    switch (key) {
    case OPT_SET_SERIAL:
	if (tool->overrides) {
	    overrides = realloc(tool->overrides,//FIXME valgrind
				strlen(tool->overrides)
				+ sizeof("," SET_SERIAL_OVERRIDE));
	    if (overrides) {
		sprintf(overrides, "%s," SET_SERIAL_OVERRIDE,
			tool->overrides, swap_endian(atoi(arg)));
		tool->overrides = overrides;
		break;
	    }
	} else {
	    tool->overrides = malloc(sizeof(SET_SERIAL_OVERRIDE));
	    if (tool->overrides) {
		sprintf(tool->overrides, SET_SERIAL_OVERRIDE, swap_endian(atoi(arg)));
		break;
	    }
	}
	fprintf(stderr, _("Unable to FIXME"));
	break;

    default:
	return ARGP_ERR_UNKNOWN;
    }
    return 0;
}



/// Data structure to communicate with argp functions
static const struct argp dummy_argp = {
    .options		= dummy_options,
    .parser		= dummy_parse_opt,
};

/// List of argument parsers to be consulted in addition
static struct argp_child dummy_argp_children[] = {
    { .argp		= &dummy_argp,
      .header		= N_("Example-specific options:"), },
    { 0 }
};



/// Implementation of generic prototype in options module
const struct argp_child*
get_custom_options(void)
{
    return dummy_argp_children;
}

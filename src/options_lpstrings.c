///@file
///@brief	Command line parsing for lpstrings utility
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
#include "find_string.h"
#include "intl.h"

#include <argp.h>
#include <string.h>
#include <stdlib.h>



///@name Option keys
///@{
#define OPT_IN_FORMAT		'I'
#define OPT_BYTES		'n'
///@}

/// Helper macro to show number literals in option help
#define _STR_MACRO(x)	_STR(x)
#define _STR(x)		#x



/// Supported command-line arguments definition
static const struct argp_option options[] = {
    { NULL,		0,		NULL,			0,
      N_("Input / output options:"),				0 },
    { "input-format",	OPT_IN_FORMAT,	N_("FORMAT"),		0,
      N_("Format of input image file.  FORMAT can be either"
	 " \"raw\""
#if HAVE_INTELHEX
	 ", \"ihex\""
#endif
	 " or \"auto\" (default)"),				0 },
    { "bytes",		OPT_BYTES,	N_("MIN-LEN"),		0,
      N_("Locate strings of at least MIN-LEN bytes in input"
	 " (argument defaults to " _STR_MACRO(FIND_STRING_DEFAULT_LENGTH)
	 " if omitted)"),					0 },
    { 0 }
};



/// Non-option arguments shown in help texts
static const char args_doc[] = N_("FILE");

/// Program short description
static const char doc[] =
    N_("Locate length-prefixed strings of printable characters plus NUL terminator.");

/// Version information for Argp
const char *argp_program_version = "lpstrings " PACKAGE_VERSION;



///@brief Argp Parser Function for command line options
///@return Zero or error code specifying how to continue
static error_t
parse_opt(
    const int key,		///< [in] Key field from the options vector
    char *arg,			///< [in,out] Value given as option argument
    struct argp_state *state)	///< [in,out] Parsing state for use by parser
{
    // Retreive the input argument from argp_parse
    struct tool_config *tool = state->input;
    const struct argp_child *child;

    switch (key) {
    case ARGP_KEY_INIT:
	if (! state->child_inputs) break;
	// Copy tool config address to be used by parser children
	for (child = state->root_argp->children; child && child->argp; ++child) {
	    state->child_inputs[child - state->root_argp->children] = state->input;
	}
	break;

    case OPT_IN_FORMAT:
	if (arg == NULL) return EINVAL;
	else if (strcmp(arg, "auto") == 0) tool->format_in = formatNone;
	else if (strcmp(arg, "raw") == 0) tool->format_in = formatRawBinary;
#if HAVE_INTELHEX
	else if (strcmp(arg, "ihex") == 0) tool->format_in = formatIntelHex;
#endif
	else argp_error(state, _("Invalid binary image format `%s' specified."), arg);
	break;

    case OPT_BYTES:
	if (arg == NULL) return EINVAL;
	else tool->locate_strings = atoi(arg);
	if (tool->locate_strings < 0 || tool->locate_strings > UINT8_MAX) {
	    argp_error(state, _("Minimum string length %d out of range."),
		       tool->locate_strings);
	    tool->locate_strings = -1;
	}
	break;

    case ARGP_KEY_ARG:	/* non-option -> input / output file name */
	// Check number of non-option arguments
	if (state->arg_num >= 1) argp_error(state, _("Too many input file arguments."));
	else tool->image_in = arg;
	break;

    case ARGP_KEY_NO_ARGS:
	argp_error(state, _("Missing file name."));
	// Fall through
    case ARGP_KEY_FINI:
	break;

    default:
	return ARGP_ERR_UNKNOWN;
    }
    return 0;
}



int
check_opts(int argc, char **argv, tool_config *config)
{
    struct argp argp = {	//top-level parser for command line
	.options	= options,
	.parser		= parse_opt,
	.args_doc	= args_doc,
	.doc		= doc,
    };
    int ret_code;

    ret_code = argp_parse(&argp, argc, argv, 0, 0, config);

    return ret_code;
}

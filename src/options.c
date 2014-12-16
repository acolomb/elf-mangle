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


#include "config.h"

#include "options.h"
#include "override.h"
#include "intl.h"

#include <argp.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#undef DEBUG
#endif



///@name Option keys
///@{
#define OPT_SECTION		'j'
#define OPT_INPUT		'i'
#define OPT_OUTPUT		'o'
#define OPT_IN_FORMAT		'I'
#define OPT_OUT_FORMAT		'O'
#define OPT_DEFINE		'D'
#define OPT_STRINGS		'l'
#define OPT_PRINT		'p'
#define OPT_ADDRESSES		'a'
#define OPT_SYMBOLS		'S'
#define OPT_FIELD_SIZE		'F'
///@}



/// Supported command-line arguments definition
static const struct argp_option options[] = {
    { NULL,		0,		NULL,			0,
      N_("Input / output options:"),	0 },
    { "section",	OPT_SECTION,	N_("SECTION"),		0,
      N_("Use SECTION from ELF file instead of default"),	0 },
    { "input",		OPT_INPUT,	N_("FILE"),		0,
      N_("Read binary input data from image FILE"),		0 },
    { "input-image",	OPT_INPUT,	NULL,			OPTION_ALIAS | OPTION_HIDDEN,
      NULL, 0 },
    { "input-format",	OPT_IN_FORMAT,	N_("FORMAT"),		0,
      N_("Format of input image file.  FORMAT can be either"
	 " \"raw\", \"ihex\" or \"auto\" (default)"),		0 },
    { "output",		OPT_OUTPUT,	N_("FILE"),		0,
      N_("Write binary data to output image FILE"),		0 },
    { "output-image",	OPT_OUTPUT,	NULL,			OPTION_ALIAS | OPTION_HIDDEN,
      NULL, 0 },
    { "output-format",	OPT_OUT_FORMAT,	N_("FORMAT"),		0,
      N_("Format of output image file.  FORMAT can be either"
	 " \"raw\" or \"ihex\" (default)"),			0 },
    { "define",		OPT_DEFINE,	N_("FIELD=BYTES,..."),	0,
      N_("Override the given fields' values (comma-separated pairs).\n"
	 "Each FIELD symbol name must be followed by an equal sign and the data"
	 " BYTES encoded in hexadecimal.  Missing bytes are left unchanged,"
	 " extra data generates an error."),	0 },

    { NULL,		0,		NULL,			0,
      N_("Display information from parsed files:"),		0 },
    { "print",		OPT_PRINT,	N_("FORMAT"),		OPTION_ARG_OPTIONAL,
      N_("Print field values.  FORMAT can be either"
	 " \"pretty\" (default) or \"hex\""),			0 },
    { "addresses",	OPT_ADDRESSES,	NULL,			0,
      N_("Print symbol address for each field"),		0 },
    { "symbols",	OPT_SYMBOLS,	NULL,			0,
      N_("Show object symbol names instead of field decriptions"), 0 },
    { "field-size",	OPT_FIELD_SIZE,	NULL,			0,
      N_("Print size in bytes for each field"),			0 },
    { "strings",	OPT_STRINGS,	N_("MIN-LEN"),		OPTION_ARG_OPTIONAL,
      N_("Locate strings of at least MIN-LEN bytes in input"),	0 },
    { 0 }
};



/// Non-option arguments shown in help texts
static const char args_doc[] = N_("IN_MAP [OUT_MAP]");

/// Program short description
static const char doc[] =
    N_("Analyze, transform and manipulate binary data based on ELF symbol tables.");

/// Version information for Argp
const char *argp_program_version = PACKAGE_STRING;



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

    case OPT_SECTION:
	tool->section = arg;
	break;

    case OPT_INPUT:
	tool->image_in = arg;
	break;

    case OPT_OUTPUT:
	tool->image_out = arg;
	break;

    case OPT_IN_FORMAT:
	if (arg == NULL) return EINVAL;
	else if (strcmp(arg, "auto") == 0) tool->format_in = formatNone;
	else if (strcmp(arg, "raw") == 0) tool->format_in = formatRawBinary;
	else if (strcmp(arg, "ihex") == 0) tool->format_in = formatIntelHex;
	else argp_error(state, _("Invalid binary image format `%s' specified."), arg);
	break;

    case OPT_OUT_FORMAT:
	if (arg == NULL) return EINVAL;
	else if (strcmp(arg, "raw") == 0) tool->format_out = formatRawBinary;
	else if (strcmp(arg, "ihex") == 0) tool->format_out = formatIntelHex;
	else argp_error(state, _("Invalid binary image format `%s' specified."), arg);
	break;

    case OPT_DEFINE:
	tool->overrides = override_append(tool->overrides, "%s", arg);
	break;

    case OPT_STRINGS:
	if (arg == NULL) tool->locate_strings = 0;
	else tool->locate_strings = atoi(arg);
	if (tool->locate_strings < 0 || tool->locate_strings > UINT8_MAX) {
	    argp_error(state, _("Minimum string length %d out of range."),
		       tool->locate_strings);
	    tool->locate_strings = -1;
	}
	break;

    case OPT_PRINT:
	if (arg == NULL || strcmp(arg, "pretty") == 0) tool->print_content = printPretty;
	else if (strcmp(arg, "hex") == 0) tool->print_content = printHex;
	else argp_error(state, _("Unknown print format `%s' specified."), arg);
	break;

    case OPT_ADDRESSES:
	tool->show_fields |= showAddress;
	break;

    case OPT_SYMBOLS:
	tool->show_fields |= showSymbol;
	break;

    case OPT_FIELD_SIZE:
	tool->show_fields |= showByteSize;
	break;

    case ARGP_KEY_ARG:	/* non-option -> input / output file name */
	// Check number of non-option arguments
	if (state->arg_num >= sizeof(tool->map_files) / sizeof(*tool->map_files))
	    argp_error(state, _("Too many map file arguments."));
	else tool->map_files[state->arg_num] = arg;
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



///@brief Access list of additional argument parsers
///@return Vector of parsers implemented by other modules
const struct argp_child*
get_custom_options(void);



int
check_opts(int argc, char **argv, tool_config *config)
{
    struct argp argp = {	//top-level parser for command line
	.options	= options,
	.parser		= parse_opt,
	.args_doc	= args_doc,
	.doc		= doc,
	.children	= get_custom_options(),
    };
    int ret_code;

    ret_code = argp_parse(&argp, argc, argv, 0, 0, config);

    return ret_code;
}

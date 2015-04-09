///@file
///@brief	Command line parsing specific to example system EEPROM images
///
/// This file is part of elf-mangle, but is not covered under its main
/// license, the GNU Lesser General Public License.  Serving as
/// example code, instead the following "unlicense" and copyright
/// waiver applies:
///
/// This is free and unencumbered software released into the public domain.
///
/// Anyone is free to copy, modify, publish, use, compile, sell, or
/// distribute this software, either in source code form or as a compiled
/// binary, for any purpose, commercial or non-commercial, and by any
/// means.
///
/// In jurisdictions that recognize copyright laws, the author or authors
/// of this software dedicate any and all copyright interest in the
/// software to the public domain. We make this dedication for the benefit
/// of the public at large and to the detriment of our heirs and
/// successors. We intend this dedication to be an overt act of
/// relinquishment in perpetuity of all present and future rights to this
/// software under copyright law.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
/// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
/// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
/// OTHER DEALINGS IN THE SOFTWARE.
///
/// For more information, please refer to <http://unlicense.org/>
///
///@author	Andre Colomb <andre.colomb@auteko.de>


#include "config.h"

#include "options.h"
#include "override.h"
#include "intl.h"

#include <argp.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>



/// Offset to distinguish example-specific options from ASCII characters
#define DUMMY_OPT_BASE		(1000)

///@name Option keys
///@{
#define OPT_SET_SERIAL		(DUMMY_OPT_BASE + 1)
///@}


/// Supported command-line arguments definition
static const struct argp_option dummy_options[] =
{
    { "set-serial",	OPT_SET_SERIAL,		N_("NUMBER"),		0,
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



/// Translate serial number argument to a symbol override
static inline int
parse_serial(char **overrides, const char *arg)
{
    int serial = 0;

    switch (sscanf(arg, "%d", &serial)) {
    case 1:
	if (serial <= 0 || serial > UINT16_MAX) return -1;
	*overrides = override_append(*overrides, "nvm_unique=%02x%02x",
				     (serial >> 0) & 0xFFU,
				     (serial >> 8) & 0xFFU);
	return 0;

    default:
	return -1;
    }
}



///@brief Argp Parser Function for example-specific command line options
///@return Zero or error code specifying how to continue
static error_t
dummy_parse_opt(
    const int key,		///< [in] Key field from the options vector
    char *arg,			///< [in,out] Value given as option argument
    struct argp_state *state)	///< [in,out] Parsing state for use by parser
{
    // Retreive the input argument from argp_parse
    struct tool_config *tool = state->input;

    switch (key) {
    case OPT_SET_SERIAL:
	if (0 != parse_serial(&tool->overrides, arg)) {
	    argp_error(state, _("Invalid serial number `%s' specified."), arg);
	}
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

///@file
///@brief	Known symbols specific to example system EEPROM images
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

#include "known_fields.h"
#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>



///@name Known data offsets and constants
///@{
static const size_t unique_serial_offset		= 0;
static const size_t unique_hardware_offset		= 2;
enum unique_hardware_type {
    hwInvalid		= 0,
    hwPrototype		= 1,
    hwProduction	= 2,
    hwMaxType		= 0x1F,
};

static const size_t version_length_offset		= 0;
static const size_t version_string_offset		= 1;
///@}



///@brief Translate hardware type specifier to descriptive string
///@return Descriptive string for hardware type
static inline const char*
get_unique_hardware_type(
    uint8_t hardware_type)	///< [in] Hardware type code from unique system identifier
{
    if (hardware_type == hwPrototype) return _("Prototype system");
    else if (hardware_type == hwProduction) return _("Production system");
    else if (hardware_type > hwMaxType) return _("invalid hardware");
    else return _("unknown hardware");
}



///@brief Pretty-print unique system identification
///@see field_print_f
static void
print_unique(const char *data, size_t size)
{
    uint16_t serial_number = 0xFFFFU;
    uint8_t hardware_type = 0xFFU;
    const char *hardware;

    if (size >= unique_serial_offset + sizeof(serial_number)) {
	serial_number = *(uint16_t*) (data + unique_serial_offset);
    }
    if (size >= unique_hardware_offset + sizeof(hardware_type)) {
	hardware_type = *(uint8_t*) (data + unique_hardware_offset);
    }

    hardware = get_unique_hardware_type(hardware_type);
    printf("S/N %02u-%05u (%s)", hardware_type, serial_number, hardware);
}



///@brief Pretty-print system firmware version string
///@see field_print_f
static void
print_version(const char *data, size_t size)
{
    /// Length of the version string field including the NUL terminator
    uint8_t version_length = 0;
    const char *version_string = NULL;

    if (size >= version_length_offset + sizeof(version_length)) {
	version_length = *(uint8_t*) (data + version_length_offset);
    }
    if (size >= version_string_offset + sizeof(const char)) {
	version_string = data + version_string_offset;
    }

    if (version_length > 0 && version_string) {		//valid data supplied
	// Check total data size boundary
	while (version_string < data + size) {
	    // Abort when given string length is reached or string is NUL-terminated
	    if (version_string >= data + version_string_offset + version_length
		|| *version_string == '\0') break;
	    putchar(*version_string);
	    ++version_string;
	}
	if (size < version_string_offset + version_length) {
	    printf(_(" (%zu of %u bytes missing)"),
		   version_string_offset + version_length - size, version_length);
	} else {
	    printf(_(" (%u bytes)"), version_length);
	}
	if (version_string >= data + version_string_offset + version_length
	    || *version_string != '\0') {
	    printf(_(" missing NUL termination!"));
	}
    } else {						//no valid data
	printf(_("<unknown version> (%u bytes)"), version_length);
    }
}



///@brief Determine symbol storage size based on stored version string length
///@see field_resize_f
static size_t
resize_version(const char *src, size_t initial __attribute((unused)))
{
    return sizeof(uint8_t) + src[version_length_offset];
}



///@brief Copy unique system identification data intelligently
///@see field_copy_f
static size_t
copy_unique(const nvm_field *field,
	    char *dst, const char *src,
	    size_t max_size)
{
    uint8_t target_hwtype = hwInvalid;

    if (max_size >= unique_hardware_offset) target_hwtype = dst[unique_hardware_offset];
    memcpy(dst, src, max_size);

    if (src[unique_hardware_offset] != target_hwtype) {
	fprintf(stderr,
		_("WARNING: %s changed to match target hardware type!\n"
		  "\t\t%02u (%s) in target map\n"
		  "\t\t%02u (%s) provided from image\n"),
		field->description,
		target_hwtype, get_unique_hardware_type(target_hwtype),
		src[unique_hardware_offset],
		get_unique_hardware_type(src[unique_hardware_offset]));
	dst[unique_hardware_offset] = target_hwtype;
	return max_size - sizeof(target_hwtype);
    }
    return max_size;
}



/// Description of known data fields for example image
static const nvm_field known_fields[] = {
    { 3,	"nvm_unique",			_("Unique system identification"),
      NULL,			copy_unique,			print_unique },
    { 0,	"nvm_version",			_("System firmware version"),
      resize_version,		copy_field_noop,		print_version }
};
/// Highest possible index in known field table
#define NUM_KNOWN_FIELDS	(sizeof(known_fields) / sizeof(*known_fields))



int
known_fields_expected(void)
{
    return NUM_KNOWN_FIELDS;
}



const nvm_field*
find_known_field(const char *symbol)
{
    return find_field(symbol, known_fields, NUM_KNOWN_FIELDS);
}

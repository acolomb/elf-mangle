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
#include <inttypes.h>



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



/// Convert 8-bit unsigned value from binary data
static inline uint8_t
convert_uint8(const char *sle)
{
    const unsigned char *ule = (const unsigned char*) sle;
    return (uint8_t) *ule;
}



/// Convert 16-bit unsigned little-endian value from binary data to host representation
static inline uint16_t
convert_uint16(const char *sle)
{
    const unsigned char *ule = (const unsigned char*) sle;
    return
	((uint16_t) ule[0] << 0) |
	((uint16_t) ule[1] << 8);
}



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
	serial_number = convert_uint16(data + unique_serial_offset);
    }
    if (size >= unique_hardware_offset + sizeof(hardware_type)) {
	hardware_type = convert_uint8(data + unique_hardware_offset);
    }

    hardware = get_unique_hardware_type(hardware_type);
    printf("S/N %02" PRIu8 "-%05" PRIu16 " (%s)", hardware_type, serial_number, hardware);
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
	version_length = convert_uint8(data + version_length_offset);
    }
    if (size >= version_string_offset + sizeof(*version_string)) {
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
	    printf(_(" (%zu of %" PRIu8 " bytes missing)"),
		   version_string_offset + version_length - size, version_length);
	} else {
	    printf(_(" (%" PRIu8 " bytes)"), version_length);
	}
	if (version_string >= data + version_string_offset + version_length
	    || *version_string != '\0') {
	    printf(_(" missing NUL termination!"));
	}
    } else {						//no valid data
	printf(_("<unknown version> (%" PRIu8 " bytes)"), version_length);
    }
}



///@brief Determine symbol storage size based on stored version string length
///@see field_resize_f
static size_t
resize_version(const char *src, size_t initial __attribute((unused)))
{
    return sizeof(uint8_t) + convert_uint8(src + version_length_offset);
}



///@brief Copy unique system identification data intelligently
///@details Keep system hardware type field intact in target data
///@see field_copy_f
static size_t
copy_unique(const nvm_field *field,
	    char *dst, const char *src,
	    size_t max_size)
{
    uint8_t target_hwtype = hwInvalid;

    if (max_size >= unique_hardware_offset) target_hwtype = convert_uint8(
	dst + unique_hardware_offset);
    memcpy(dst, src, max_size);

    if (convert_uint8(src + unique_hardware_offset) != target_hwtype) {
	fprintf(stderr,
		_("WARNING: %s changed to match target hardware type!\n"
		  "\t\t%02" PRIu8 " (%s) in target map\n"
		  "\t\t%02" PRIu8 " (%s) provided from image\n"),
		field->description,
		target_hwtype, get_unique_hardware_type(target_hwtype),
		convert_uint8(src + unique_hardware_offset),
		get_unique_hardware_type(convert_uint8(src + unique_hardware_offset)));
	dst[unique_hardware_offset] = target_hwtype;
	return max_size - sizeof(target_hwtype);
    }
    return max_size;
}



/// Description of known data fields for example image
static const nvm_field known_fields[] = {
    { 3,	"nvm_unique",			N_("Unique system identification"),
      NULL,			copy_unique,			print_unique },
    { 0,	"nvm_version",			N_("System firmware version"),
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

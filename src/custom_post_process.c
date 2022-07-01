///@file
///@brief	Image post-processing specific to example system EEPROM images
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
///@author	Andre Colomb <src@andre.colomb.de>


#include "config.h"

#include "post_process.h"
#include "symbol_list.h"
#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <inttypes.h>

/// Compile diagnostic output messages?
#define DEBUG 0



// Forward declarations
void post_process_disable_checksum_update(void);



/// Convert 32-bit unsigned little-endian value from binary data to host representation
static inline uint32_t
convert_uint32(const char *sle)
{
    const unsigned char *ule = (const unsigned char*) sle;
    return
	((uint32_t) ule[0] << 0) |
	((uint32_t) ule[1] << 8) |
	((uint32_t) ule[2] << 16) |
	((uint32_t) ule[3] << 24);
}



/// Reverses (reflects) bits in a 32-bit word.
static inline uint32_t
reverse(uint32_t x) {
    x = ((x & 0x55555555U) << 1) | ((x >> 1) & 0x55555555U);
    x = ((x & 0x33333333U) << 2) | ((x >> 2) & 0x33333333U);
    x = ((x & 0x0F0F0F0FU) << 4) | ((x >> 4) & 0x0F0F0F0FU);
    x = (x << 24) | ((x & 0x0000FF00U) << 8) |
	((x >> 8) & 0x0000FF00U) | (x >> 24);
    return x;
}



/// CRC-32 calculation with some optimization but no table lookup
static inline uint32_t
crc32(const unsigned char *message, const size_t size,
      const size_t skip_offset, const size_t skip_size)
{
    const uint32_t init = 0xFFFFFFFF, poly = reverse(0x04C11DB7);
    uint32_t byte, crc = init, mask;

    for (size_t i = 0; i < size; ++i) {
	if (skip_size && i == skip_offset) {
	    i += skip_size - 1;
	    if (DEBUG) printf("skipping %#zx to %#zx\n", skip_offset, i);
	    continue;
	}
	byte = message[i];
	if (DEBUG) printf("reading %" PRIx8 " from %#zx\n", byte, i);
	crc = crc ^ byte;
	for (int j = 7; j >= 0; --j) {
	    mask = -(crc & 1U);
	    crc = (crc >> 1) ^ (poly & mask);
	}
    }
    if (DEBUG) printf("CRC result %" PRIX32 "\n", ~crc);
    return ~crc;
}



/// Recalculate, verify, and optionally update stored CRC value
static inline int
check_crc_symbol(const char* blob, size_t blob_size,
		 const nvm_symbol *list, const int size,
		 int check_only)
{
    typedef uint32_t nvm_crc_t;
    const char crc_symbol[] = "nvm_crc";
    const nvm_symbol *target;
    nvm_crc_t crc;
    size_t copied;

    target = symbol_list_find_symbol(list, size, crc_symbol);
    if (! target) {
	fprintf(stderr, _("Checksum field %s not found in map.\n"), crc_symbol);
	post_process_disable_checksum_update();
	return 0;
    }
    if (target->size != sizeof(nvm_crc_t)) {
	fprintf(stderr, _("Checksum field %s has %zu bytes, expected %zu.\n"),
		crc_symbol, target->size, sizeof(nvm_crc_t));
	post_process_disable_checksum_update();
	return -1;
    }

    crc = crc32((const unsigned char*) blob, blob_size, target->offset, target->size);
    // Forced little-endian byte order
    const unsigned char crc_bytes[sizeof(nvm_crc_t)] = {
	(crc >> 0) & 0xFFU,
	(crc >> 8) & 0xFFU,
	(crc >> 16) & 0xFFU,
	(crc >> 24) & 0xFFU,
    };

    int diff = memcmp(target->blob_address, crc_bytes, sizeof(crc_bytes));
    if (! diff) {
	if (check_only) fprintf(stderr, _("CORRECT checksum %08" PRIX32 " in field %s.\n"),
				convert_uint32(target->blob_address), crc_symbol);
	return 0;
    } else if (check_only) {
	fprintf(stderr, _("WRONG checksum %08" PRIX32 " in field %s,"
			  " expected %08" PRIX32 ".\n"),
		convert_uint32(target->blob_address), crc_symbol, crc);
	return -2;
    }

    copied = copy_field_verbatim(NULL, target->blob_address, (const char*) crc_bytes,
				 target->size, sizeof(crc_bytes));
    if (copied != target->size) {
	fprintf(stderr, _("Failed to update checksum field %s to %08" PRIX32 ".\n"),
		crc_symbol, crc);
	return -3;
    } else {
	fprintf(stderr, _("Updated checksum field %s to %08" PRIX32 ".\n"),
		crc_symbol, crc);
	return 1;
    }
}



///@brief Recalculate CRC and compare against stored value
///@see post_process_f
static int
verify_crc(const char* blob, size_t blob_size,
	   const nvm_symbol *list, const int size)
{
    return check_crc_symbol(blob, blob_size, list, size, 1);
}



///@brief Recalculate and update stored CRC value
///@see post_process_f
static int
update_crc(const char* blob, size_t blob_size,
	   const nvm_symbol *list, const int size)
{
    return check_crc_symbol(blob, blob_size, list, size, 0);
}



/// List of post processing functions to be consulted
static post_process_f post_processors[] = {
    verify_crc,
    update_crc,
    NULL
};
/// Highest possible index in known field table
#define NUM_POST_PROCESSORS	(sizeof(post_processors) / sizeof(*post_processors))



/// Implementation of generic prototype in post_process module
const post_process_f*
get_custom_post_processors(void)
{
    return post_processors;
}



/// Remove the CRC update post-processor from the list of functions to be called
void
post_process_disable_checksum_update(void)
{
    for (post_process_f *f = post_processors; f < post_processors + NUM_POST_PROCESSORS; ++f) {
	if (*f == update_crc) {
	    *f = 0;
	    return;
	}
    }
}

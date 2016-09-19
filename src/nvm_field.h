///@file
///@brief	Description of data fields associated with symbols
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
///@author	Andre Colomb <src@andre.colomb.de>


#ifndef NVM_FIELD_H_
#define NVM_FIELD_H_

#include <stddef.h>


// Forward declaration
typedef struct nvm_field nvm_field;


///@brief Function pointer to adjust storage size for a symbol
///@return Actual size extent of the symbol data based on content information
typedef size_t (*field_resize_f)(
    const char *src,		///< [in] Data field content
    size_t initial		///< [in] Currently known symbol size in bytes
    );

///@brief Function pointer to copy individual field data intelligently
///@return Number of bytes copied
typedef size_t (*field_copy_f)(
    const nvm_field *field,	///< [in] Access to field descriptor //FIXME needed?
    char *dst,			///< [out] Destination address for field data
    const char *src,		///< [in] Source address for field data
    size_t dst_size,		///< [in] Maximum size in bytes to write to destination
    size_t src_size		///< [in] Maximum size in bytes available from source
);

///@brief Function pointer to pretty-print data field content
typedef void (*field_print_f)(
    const char *data,		///< [in] Data field content
    size_t size			///< [in] Size of the data block in bytes
    );

/// Description of a binary data field
struct nvm_field {
    /// Default data size in bytes (may differ between values), zero for unknown
    size_t		expected_size;
    /// Name of the map symbol to locate the data field in blobs
    const char*		symbol;
    /// Textual description of what the data represents
    const char*		description;
    /// Function to determine storage size differing from expected, NULL for none
    field_resize_f	resize_func;
    /// Function to copy data intelligently, NULL to use copy_field_verbatim()
    field_copy_f	copy_func;
    /// Function to pretty-print data field content in human-readable form
    field_print_f	print_func;
};

///@brief Find the field descriptor matching a symbol name
///@return Address of the field descriptor or NULL on error
const nvm_field* find_field(
    const char *symbol,		///< [in] Symbol name to look for
    const nvm_field fields[],	///< [in] Vector of field descriptors
    size_t num_fields		///< [in] Number of elements in the vector
    );

///@brief Copy data field content byte-wise
///@see field_copy_f
size_t copy_field_verbatim(
    const nvm_field *field,	///< [in] Access to field descriptor //FIXME needed?
    char *dst,			///< [out] Destination address for field data
    const char *src,		///< [in] Source address for field data
    size_t dst_size,		///< [in] Maximum size in bytes to write to destination
    size_t src_size		///< [in] Maximum size in bytes available from source
    );

///@brief Do not copy data field content (dummy implementation)
///@see field_copy_f
size_t copy_field_noop(
    const nvm_field *field,	///< [in] Access to field descriptor //FIXME needed?
    char *dst,			///< [out] Destination address for field data
    const char *src,		///< [in] Source address for field data
    size_t dst_size,		///< [in] Maximum size in bytes to write to destination
    size_t src_size		///< [in] Maximum size in bytes available from source
    );

#endif //NVM_FIELD_H_

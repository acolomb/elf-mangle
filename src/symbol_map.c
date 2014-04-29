///@file
///@brief	Parsing of ELF files as symbol map sources
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

#include "symbol_map.h"
#include "symbol_list.h"
#include "known_fields.h"
#include "field_list.h"
#include "nvm_field.h"
#include "intl.h"

#include <gelf.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef DEBUG
#undef DEBUG
#endif



/// Internal state of a symbol map
struct nvm_symbol_map_source {
    /// File descriptor for the ELF file
    int			fd;
    /// Handle to process the file with libelf
    Elf*		elf;
    /// Address of the binary data
    char*		blob;
    /// Size of the binary data
    size_t		blob_size;
};



/// List of unknown fields found during parsing
static nvm_field_list fields_unknown;



///@brief Examine the overall ELF structure to find needed sections
static void
find_symtab_and_section(
    Elf *elf,			///< [in] Elf object handle
    Elf_Scn **symtab,		///< [out] Section handle for the symbol table
    size_t *strings_index,	///< [out] Number of the string table section
    const char *section_name,	///< [in] Name of the data section to examine
    Elf_Scn **section,		///< [out] Section handle for the requested data section
    GElf_Shdr *header)		///< [out] Section header of the requested data section
{
    size_t shstrndx;
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;

    // Require all output arguments
    if (! symtab || ! section || ! header) return;
    *symtab = *section = NULL;
    // Get the section header string table index
    if (elf_getshdrstrndx(elf, &shstrndx) < 0) {
	fprintf(stderr, _("Could not access section header string table: %s\n"),
		elf_errmsg(elf_errno()));//FIXME
	return;
    }

    // Scan through the section table
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
	// Get the section header
	if (gelf_getshdr(scn, &shdr) != NULL) {
#ifdef DEBUG
	    fprintf(stderr, _("%s: [%zu] %s\n"), __func__,
		    elf_ndxscn(scn), elf_strptr(elf, shstrndx, shdr.sh_name));
#endif
	    if (shdr.sh_type == SHT_SYMTAB) {
		*symtab = scn;
		*strings_index = shdr.sh_link;
	    } else if (strcmp(elf_strptr(elf, shstrndx, shdr.sh_name),
			      section_name) == 0) {
		*section = scn;
		*header = shdr;
	    }
	    if (*symtab && *section) break;
	} else {
	    fprintf(stderr, _("Header of ELF section %zu inaccessible: %s\n"),
		    elf_ndxscn(scn), elf_errmsg(elf_errno()));
	}
    }
}



///@brief Allocate memory for the section's binary data
///@return Address of the allocated memory or NULL on error
static char*
allocate_blob(
    nvm_symbol_map_source *source,	///< [in,out] Handle of the map source
    GElf_Shdr *header)			///< [in] Section header of the data section
{
    if (! header) return NULL;

    // Allocate fresh blob memory because ELF object data may be memory mapped
    source->blob_size = header->sh_size;
    source->blob = malloc(source->blob_size);
    if (! source->blob) fprintf(stderr, _("Could not allocate image data: %s\n"),
				strerror(errno));
    return source->blob;
}



///@brief Parse ELF symbol table and extract information about data section
///@return
/// - Number of symbols parsed successfully
/// - Zero if no symbols were found
/// - Negative value on error
static int
parse_elf_symbols(
    Elf *elf,			///< [in] Elf object handle
    Elf_Scn *symtab,		///< [in] Section handle for the symbol table
    size_t strings_index,	///< [in] Number of the string table section
    Elf_Scn *section,		///< [in] Section handle for the requested data section
    GElf_Shdr *header,		///< [out] Section header of the requested data section
    nvm_symbol **symbol_list,	///< [out] List of discovered symbols
    char *blob_data)		///< [out] Binary data contained in ELF section
{
    int symbol_count = 0, sym_index, syms_total, list_size;
    Elf_Data *symtab_data, *section_data;
    GElf_Sym sym;
    nvm_symbol *current;

    // Require both output arguments
    if (! symbol_list || ! blob_data) return -1;

    // Get the symbol table and section data
    if (! (symtab_data = elf_getdata(symtab, NULL))) return -2;
    if (! (section_data = elf_rawdata(section, NULL))) return -2;
    // Initialize blob with default data from section content
    memcpy(blob_data, section_data->d_buf, section_data->d_size);

    // Calculate the number of entries in the symbol table
    syms_total = symtab_data->d_size / gelf_fsize(elf, ELF_T_SYM, 1, EV_CURRENT);
    // Pre-allocate symbol list with number of expected entries
    list_size = known_fields_expected(); //FIXME check handling when fewer symbols are found
    if (! (*symbol_list = calloc(list_size, sizeof(nvm_symbol)))) return -3;
    current = symbol_list[0];

    for (sym_index = 0; sym_index < syms_total; ++sym_index) {
	if (! gelf_getsym(symtab_data, sym_index, &sym) ||	//no symbol found
	    sym.st_shndx != elf_ndxscn(section) ||		//symbol in wrong section
	    sym.st_size == 0) continue;				//empty symbol
	if (symbol_count >= list_size) {	//list is full
	    if (! symbol_list_append(symbol_list, &list_size)) continue;//FIXME
	}
	current = (*symbol_list) + symbol_count++;
	current->offset = sym.st_value - header->sh_addr;
	current->size = sym.st_size;
	current->blob_address = blob_data + current->offset;
	current->field = find_known_field(elf_strptr(elf, strings_index, sym.st_name));
	// Look up field in case it was found during previous parsing
	if (! current->field) current->field = field_list_find(
	    elf_strptr(elf, strings_index, sym.st_name), &fields_unknown);
	if (! current->field) {		//unknown symbol encountered
	    current->field = field_list_add(
		&fields_unknown, sym.st_size,
		elf_strptr(elf, strings_index, sym.st_name), NULL);
	}
	// Update symbol size based on content where applicable
	if (current->field && current->field->expected_size != current->size
	    && current->field->resize_func) {
	    current->size = current->field->resize_func(current->blob_address, current->size);
	    // Limit to blob boundary
	    if (current->size > section_data->d_size - current->offset)
		current->size = section_data->d_size - current->offset;
	}
    }
    return symbol_count;
}



nvm_symbol_map_source*
symbol_map_open_file(const char *filename)
{
    nvm_symbol_map_source *source;
    const char *errmsg = "";

    if (! filename) return NULL;

    source = malloc(sizeof(*source));
    if (source) {
	source->fd = open(filename, O_RDONLY);
	source->elf = NULL;
	source->blob = NULL;
	source->blob_size = 0;

	if (source->fd != -1) {
	    elf_version(EV_CURRENT);
	    source->elf = elf_begin(source->fd, ELF_C_READ, NULL);
	    if (source->elf) {
		if (elf_kind(source->elf) == ELF_K_ELF) {
		    // Skip any clean-up code below if successful
		    return source;
		} else errmsg = _("Not an ELF object");	//wrong object kind
		elf_end(source->elf);
	    } else errmsg = elf_errmsg(elf_errno());	//ELF object not opened
	    close(source->fd);
	} else errmsg = strerror(errno);		//file not opened
	free(source);
    } else errmsg = strerror(errno);			//malloc() failed
    // All cleaned up after error

    fprintf(stderr, _("Cannot open symbol map \"%s\" (%s)\n"), filename, errmsg);
    return NULL;
}



int
symbol_map_parse(nvm_symbol_map_source *source,
		 const char *section_name,
		 nvm_symbol **symbol_list)
{
    size_t string_index;
    Elf_Scn *symtab, *section;
    GElf_Shdr header;

    if (! source) return -1;

    find_symtab_and_section(source->elf, &symtab, &string_index,
			    section_name, &section, &header);
    if (! section || ! symtab) return -2;

    if (! allocate_blob(source, &header)) return -3;

    return parse_elf_symbols(source->elf, symtab, string_index,
			     section, &header,
			     symbol_list, source->blob);
}



char*
symbol_map_blob_address(nvm_symbol_map_source *source)
{
    if (! source) return NULL;
    return source->blob;
}



size_t
symbol_map_blob_size(nvm_symbol_map_source *source)
{
    if (! source) return 0;
    return source->blob_size;
}



void
symbol_map_close(nvm_symbol_map_source *source)
{
    if (source) {
	free(source->blob);
	if (source->elf) elf_end(source->elf);
	if (source->fd >= 0) close(source->fd);
    }
    free(source);
}

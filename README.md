
elf-mangle - ELF Binary Image Manipulator
=========================================

A command line tool to analyze, transform and manipulate binary data
based on ELF symbol tables.

Author: André Colomb <src@andre.colomb.de>


License
-------

Copyright (C) 2014, 2015, 2016, 2019, 2021, 2022, 2023  André Colomb

elf-mangle is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

elf-mangle is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.


Motivation
----------

Compilers and linkers do a great job in generating machine-readable
binary code to be run on a CPU or embedded microprocessor.  After
these tools have finished their job, sometimes it is necessary to
alter some bits of information inside these binary files.  The most
common modern executable file format (at least in the UNIX / Linux
world) is **ELF**, the Executable and Linkable Format.

The *elf-mangle* tool exists to examine and alter (thus *mangle*)
these ELF executable and binary data images, for example when you need
an embedded serial number or calibration data which is unique.  It
allows you to process the data using associated ELF files as "lenses"
or maps describing the information layout and possibly semantics.

The use-case why the tool was written is an embedded controller for a
mechanical system whose firmware contains a serial number in the
controller's EEPROM memory.  Thus, the default ELF section examined by
*elf-mangle* is named `.eeprom`.  It also serves as a migration tool
for firmware upgrades, where the calibration data saved by the system
needs to be read from the EEPROM, converted to match the new version's
storage layout, and written back to an image file ready for flashing.


### Features ###

+ Extract symbol content for arbitrary ELF object file sections.
+ Dump data behind an ELF symbol, possibly with pretty-print function.
+ Show symbol address offsets within the section's binary data.
+ Override data for symbols in output:
  * Specified on the command line
  * Read from a text file with value assignments
  * Loaded from an existing binary data image (blob)
+ Modular support for different input / output image formats:
  * Intel Hex encoding (requires [libcintelhex][ihex-fork])
  * Raw binary data
+ Load a second ELF object to transcribe matching symbols between
  input and output layout.
+ Search for special printable string structures within binary data
+ Code is designed for easy application-specific extensions:
  * Additional command line options
  * List of known symbol names with custom functions for
	- Pretty-printing
	- Copying data from input to output image
	- Overriding the symbol data size in case the ELF file reports a
      wrong value (such as with flexible array member in C structs)
  * Post-processing steps to apply application-specific content rules


Installation
------------

The elf-mangle build system uses the *GNU autotools* - `autoconf`,
`automake`, `gettext`, ...  When using the release version archives,
it's as simple as the usual:

	./configure
	make
	sudo make install

Additional options can be specified for `configure`, see below for
details.  When cloning from [the GitHub repo][github], additional
bootstrapping needs to be carried out.

[github]: https://github.com/acolomb/elf-mangle "Project repository on GitHub"


### Bootstrapping ###

Setting up the build system from the repository contents needs
additional developer tools installed.  The following versions are
tested:

- GNU autoconf 2.69
- GNU automake 1.16
- GNU libtool 2.4.6
- GNU gettext 0.19.8

To fully generate the build system, run these three commands in order:

	git submodule update --init		# embedded dependencies
	gnulib-tool --update			# optional, see portability notes below
	autoreconf -i -f


### Compilation ###

There are many options controlling the GNU Build System
a.k.a. *autotools*.  Please refer to the respective documentation,
especially the [GNU Autoconf manual][autoconf].  Some specific options
for the `configure` script are:

* `--enable-custom-options`: Include code module providing custom,
  application specific command line options
* `--enable-custom-fields`: Include code module providing descriptions
  of known application specific symbols with advanced processing.
* `--enable-custom-post-process`: Include code module providing
  post-processing functions to make further finalizing modifications
  to an image.

See the section "Application Extensions" below for more information on
the latter.  For more options, refer to the output of `./configure
--help`.  After configuration, the package can be built with `make
all` and installed with `make install` (with sufficient file
permissions).

[autoconf]: https://www.gnu.org/software/autoconf/manual/ "GNU Autoconf manual"


### Dependencies ###

*elf-mangle* has some required and some optional dependencies.


#### Required: libelf ####

Dealing with ELF files is done through the standard *libelf* library,
therefore a compatible version must be present.  Several different
implementations are available, with varying licenses.

A pretty stable implementation is maintained at
[sourceware.org][elfutils], as part of the *elfutils* suite, and is
dual-licensed under the *GPLv2+* / *LGPLv3+*.  It is
[packaged][elfutils-deb] in most *Debian*-based Linux distributions
and can be compiled under Win32 using *MinGW*.

[elfutils]: https://sourceware.org/elfutils/
[elfutils-deb]: https://tracker.debian.org/pkg/elfutils

The [ELF Tool Chain Project][elftoolchain] includes another *libelf*
implementation which is *BSD*-licensed.  A corresponding *Debian*
[package][libelf-freebsd] is built from their sources.

[elftoolchain]: https://sourceforge.net/projects/elftoolchain/
[libelf-freebsd]: https://packages.debian.org/unstable/libelf-freebsd-dev

Up until about 2019, an excellent *LGPL*-licensed implementation,
written by Michael Riepe, used to be found at
[www.mr511.de][libelf-lgpl].  It was [included][libelfg0] in most
*Debian*-based Linux distributions as well as *Cygwin* and can be
compiled under Win32 using *MinGW*.

[libelf-lgpl]: http://www.mr511.de/software/ "libelf LGPL'ed"
[libelfg0]: https://tracker.debian.org/pkg/libelf "libelf in Debian"

Either implementation should work if it supports the BSD *GElf* API.
Just make sure the compiler finds the required headers and library.
If not installed in a standard location, suitable flags should be
passed through the `CPPFLAGS` and `LDFLAGS` environment variables when
running `configure`.


#### Optional: libcintelhex ####

For reading blob data from *Intel Hex* formatted files, the program
depends on the freely available, *LGPL*-licensed *libcintelhex*
library.  Some API extensions needed by *elf-mangle* currently live in
a [fork at GitHub][ihex-fork].  The [upstream version][ihex-orig] is
therefore not compatible yet.

The library should be cloned from GitHub and built before configuring
*elf-mangle*.  If it is not installed system-wide, the `CPPFLAGS` and
`LDFLAGS` environment variables may be used to point `configure` to
the right location.  The command-line option `--with-cintelhex=`
accepts an installation prefix to correctly set up the flags variables
automatically.  To force building without *Intel Hex* support, specify
`--with-cintelhex=no`.

For convenience, the Git repository includes a submodule definition
pointing at the forked repository.  Initialize and use it with

	git submodule update --init libcintelhex
	./configure --with-cintelhex=internal

[ihex-fork]: https://github.com/acolomb/libcintelhex "Fork of libcintelhex with extended API"
[ihex-orig]: https://github.com/martin-helmich/libcintelhex "Original libcintelhex project"


### Portability ###

Some functionality provided by the *GNU C Library* and used by
*elf-mangle* is not available on other platforms.  The compatibility
layer [GNU Gnulib][gnulib] can be used to replace the missing bits.
The tarball includes the relevant *Gnulib* sources, so the following
step is only necessary when building from the repository.

A configuration file for *gnulib-tool* is available in the repository.
If you have a *Gnulib* source checkout, copy the needed modules with
the following commands:

	gnulib-tool --update
	autoreconf -i -f

[gnulib]: http://www.gnu.org/software/gnulib/ "Gnulib home page"


Usage
-----

*elf-mangle* is completely controlled from its command line.  The
output of `elf-mangle --help` shows a summary of all available
options.  The general structure looks like:

	elf-mangle [OPTION...] IN_MAP [OUT_MAP]

Options can be placed anywhere on the command line and their order
does not matter.  However, the order of the (ELF) map files must
always be input first, output last.


### Input Layout Map ###

At least one argument is required, which must be the ELF object file
used as input map.  Its symbol table is scanned for symbols in the
section to be examined, which is by default named `.eeprom`, but can
be overridden using the `--section=SECTION` option.  Other useful
section names might be `.data` or `.rodata` for example--wherever the
compiler has placed interesting information.

Given only an input ELF file name, *elf-mangle* just tries to parse
the symbol table and then exits.  To display the symbols found, use
the `--print` option.  It defaults to `pretty` mode, calling any print
function *elf-mangle* has for known symbols (see the section
"Application Extensions" below for how to use them).  Since by default
no symbol has a known special meaning, only the symbol names are
listed.  Use `--print=hex` to generate a hex dump of the data content
for each symbol.  The `--print=defines` variant outputs a format
suitable for later use with the `--define-from` option (see below).

Optionally the displayed symbol list can be filtered using the
`--changed` option.  Symbols whose value in the output is unchanged
from what the ELF file contained, after applying any transformations,
overrides and post-processing (see below), are skipped from display.

The options `--addresses` and `--field-size` may be used to show
additional information about the data offset within the section and
the size in bytes for each symbol.  Add the `--section-size` option to
print out the byte size of all data in the examined section.  The
`--symbols` option causes the symbol names to be displayed, even for
known symbols with a description.  It also sets the `--section-size`
output prefix to simply `total: ` (not localized).


### Input Blob ###

When reading an ELF object file, *elf-mangle* creates an in-memory
image of the requested section contents, which we shall call a *blob*.
But not only the contents of the ELF object file can be examined.
Given a data dump of the corresponding ELF section with the `--input`
option, *elf-mangle* can examine its contents according to the input
layout map.  For each input symbol, the corresponding content range
from the blob replaces the data found in the input ELF object.

Note that this is not quite the same as copying the whole section
data.  Only data ranges pointed to by an ELF symbol will be copied.
This must be considererd when working with C's flexible array member
feature, for example, where the symbol size may not include all data
stored for a variable.


### Transforming Output Layout ###

One main use of *elf-mangle* is transforming blob data between
different storage layouts.  Symbols in the section may have moved in a
newer program version, for example, and existing data should be
transformed to be used by the newer version.

Given an `IN_MAP` and an `OUT_MAP` argument, each symbol in the output
map is matched against the input's symbols.  By default, the input
blob data is then copied to the output, but only as many bytes as
recorded in the output symbol's size descriptor.  Application
Extensions can be used to provide more complicated copying rules on a
per-symbol basis.

Any section content not affected by copying from the input retains its
data as contained in the ELF object specified as `OUT_MAP`.  By use of
the `--print` option, the transformed contents can be examined.  Note
that the displayed symbol list now concerns the **output** file, in
contrast to the previous use-case when the `OUT_MAP` argument was
omitted.  This applies to the `--section-size` option as well.

In general, *elf-mangle* without an `OUT_MAP` argument behaves just as
if the same file was given for `IN_MAP` and `OUT_MAP`.


### Overriding Fields ###

After copying the ELF section data, *elf-mangle* allows to override
the contents of individual symbols in the output.  This can be useful
for example when compiling a program for an embedded controller only
once, but then generating several flash images containing individual,
unique serial numbers.

The `--define` option takes a comma-separated list as argument,
consisting of field-value pairs.  The field is specified by its ELF
symbol name, followed by an equal sign and as many bytes as should be
overridden.  The data bytes must be encoded as two-digit hexadecimal
numbers.  Example:

	elf-mangle --define foo=beef4a11,bar=42

Extra data (more than the symbol's size) is silently ignored.  Fewer
bytes only override the start of the symbol's content range.

The same field-value pairs can be read from a text file as well, given
with the `--define-from` option.  The definitions may be separated by
commas and / or on separate lines.  Files previously written by the
`--print=defines` option are suitable as input for this, providing an
easy way of serializing parameters.  A single dash (`-`) as filename
specifies reading the definitions from the standard input stream.

Directives read from a file are processed first, and possibly
overridden by matching definitions on the command line.  The
`--define` options are processed in the order given, so for repeated
symbol names the last one wins.  Specifying the option repeatedly is
possible, just as listing several definitions in one argument,
comma-separated.


### Output Blob ###

The transformations described above are not very useful unless the
output section data can actually be saved back to a file--which is of
course supported.  Note that *elf-mangle* will **never** alter any of
the ELF object files given as arguments!

In the simplest case of only an input map file, the blob data can
simply be written to an output image file specified with the
`--output` option.  The same functionality could be achieved with the
`objcopy` tool, with a load of supported binary formats.  However,
transforming the object layout and mangling the data for individual
symbols is not easy with that tool.

Just as when printing out the symbol list, the blob data that is
written to the output image file inherits its layout (as well as
content ranges not addressed by any symbol) from the output ELF object
file if one was specified, otherwise from the input map.


### Blob Formats ###

For reading and writing blob data from / to image files, *elf-mangle*
currently supports raw binary or Intel Hex files.  Other formats may
be added later, as the code is kept modular.

The output image file format can be chosen with the `--output-format`
option.  **Writing Intel Hex files** is supported even without the
optional *libcintelhex* dependency.

For input image files, the `--input-format` option determines how it
is interpreted.  Without any option or when specifying `auto`, the
file is parsed as an Intel Hex file, falling back to raw binary
interpretation in case of errors.  Specify `ihex` or `raw` to force
the respective format.  Note that *libcintelhex* is required for **Intel
Hex file reading** support.


### Special Strings ###

In addition to the features described above, *elf-mangle* includes an
additional function which may be helpful for some typical use cases.
It was tacked-on to this utility to scratch an itch with the original
application, an embedded firmware upgrade helper.

Such a firmware image had a signature string embedded within the
binary, which always reflects the firmware version.  When dealing with
firmware upgrades, it became necessary to verify whether a given input
blob (in this case an EEPROM image with configuration values) actually
matches the input layout.

For this purpose, the signature string was embedded in the blob data
in a special representation to easily locate and verify it later.  It
consists of a single unsigned byte whose value specifies the length of
the following string including its *NUL* terminator.  The string data
immediately follows this length field and may only consist of
printable characters, plus the terminating *NUL* byte.  Therefore, the
offset from the length field to the *NUL* byte must be exactly as many
bytes as the length field indicates.  An example declaration in C of
such a special string structure (with a C99 flexible array member) may
look like this:

	struct special_string {
		unsigned char	length;
		const char		data[];
	} foo_string = { sizeof("bar"), "bar" };

The `--strings` option tells *elf-mangle* to search for such specially
constructed strings within the input blob data.  It may be followed by
a byte count specifying the minimum string length to look out for.
Only strings matching the above description with a length greater than
or equal to the given minimum will be listed, including their offset
within the image and actual length.  The `--symbols` option, as a side
effect, forces a consistent output format starting with `lpstring`,
which is not localized and thus suitable for further processing.

The string searching algorithm is similar to the standard UNIX
`strings` utility.  The latter however does not handle the special
format with a length prefix byte, and usually operates on all
initialized and loaded sections of the object file.


#### Length-Prefixed Strings in Binary Image Files ####

Similar to the standard POSIX `strings(1)` utility, *elf-mangle*
includes a separate binary called `lpstrings` that applies the search
algorithm described above to a given binary data image. In contrast to
`elf-mangle`, it works without an ELF file as input, but directly
scans a binary file in any supported format, as documented for the
`--input-format` option.

The default output format used by `lpstrings` is the same as for
*elf-mangle*, but can be simplified if either the `--output-separator`
or `--radix` options are given.  The former allows specifying a custom
delimiter between located strings, which is a newline by default.  If
given, the `--radix` option causes the start offset to be prepended to
each string, as an octal (`o`), decimal (`d`) or hexadecimal (`x`)
number.

As an extension to the POSIX options, `lpstrings` may print the
string length in bytes without NUL terminator for each string if the
`--length` option is given in addition to `--radix`.  It is appended
after the offset with a plus sign and uses the same radix.

In contrast to POSIX `strings(1)`, processing the standard input or
more than one file given on the command line is not possible.  A
positive exit status indicates the number of strings found, negative
values indicate errors.


Application Extensions
----------------------

*elf-mangle* tries to cover as many use-cases as possible with its
standard functions and provide a general-purpose tool.  For an
application with special needs, the code can be easily extended to
provide additional semantics or shortcut options supplementing the
basic program logic.

Three source code files are provided to showcase the possible
flexibility.  They will not be compiled by default and must first be
enabled with the `configure` script switches `--enable-custom-fields`,
`--enable-custom-options` and `--enable-custom-post-process`,
respectively. The sources are shipped within the distribution as:

	src/custom_known_fields.c
	src/custom_options.c
	src/custom_post_process.c

It is recommended to modify and adapt these files to the needs of the
respective application.  To ease later upgrades and track changes in
*elf-mangle* development, these changes should be maintained within a
git branch for example.

When building and installing a customized, application-specific
version of *elf-mangle*, the `--program-prefix` option to `configure`
may come in handy to install the custom program binary with a
different name.  This way, the original *elf-mangle* binary can
coexist nicely.


### Describing Known Fields ###

The "known fields" mechanism is used to attach some meaning and
semantics to specific symbol names within the layout map files.  It
must provide a function `find_known_field()` to return the address of
a descriptor structure for a given symbol name, or `NULL` if the
symbol is unknown.

The descriptor structure contains the following elements:

1. The expected field data size in bytes
2. The associated symbol name
3. A human-readable descriptive field title displayed instead of the
   symbol name when listing contents
4. Functions overriding some aspects of the program logic:
   * Determining the field's data size in case the symbol size does
     not match the amount of data to be interpreted.  This may be the
     case with C99 flexible array members for example, where the
     compiler may not know the amount of data stored for a variable in
     advance from its declaration.  The symbol size in this case would
     exclude the flexible array member.
   * Copying the field's content from input to output blob with
     special handling.  This can also be used to specify a no-op
     function and never copy content for a given field.
   * Pretty-printing the field's contents instead of just dumping hex
     bytes.  For example, the print function could output a symbol
     with four bytes interpreted as a signed 32-bit integer.

The included dummy implementation includes examples for all of these
functions, based on a real-world application for firmware EEPROM
upgrades.  The example `resize_version()` function deals with a
signature string formatted as described above, which could also be
examined using the `--strings` option.

To provide the program logic with an estimate of how many symbols may
be found within a layout map, the `known_fields_expected()` function
should return the number of known fields.  This avoids having to
dynamically resize the list of symbols when scanning an ELF file.


### Extending Command Line Options ###

The command line arguments are parsed using the *Argp* functions from
the [GNU C Library][glibc].  Those include an easy way to extend the
option handling with separate parsers for different groups of options.

[glibc]: http://www.gnu.org/software/libc/ "The GNU C Library (glibc)"

To implement application-specific command line extensions, a function
named `get_custom_options()` should be provided in `custom_options.c`,
returning a pointer to an `argp_child` structure, as described in the
[glibc][glibc] manual.  This child parser will be added to the
program's own options handling.

One of the most obvious use-cases for these extensions is to provide
options for overriding single fields, but not with a string of hex
bytes, but with human-readable values.  The example implementation
includes such an option to override a unique serial number within the
blob data.  The same effect could be achieved using the `--define`
option, but without plausibility checks and without the convenience of
a human-readable number representation.


### Adding Post-Processing Functions ###

Post-processors in *elf-mangle* allow arbitrary validation and
adjustment steps to be carried out on the output binary image.  They
have access to the complete blob data, as well as the list of
contained fields, with all layout transformations and overrides
already applied.  Any changes made in a post-processor will affect
what is written to the output image file and also pretty-printed on
request.

To use this, a function named `get_custom_post_processors()` should be
provided in `custom_post_process.c`, returning a `NULL`-terminated
list of `post_process_f` function pointers.  These functions will be
called in the listed order, each with the resulting blob from the
previous one.  The first `NULL` value in the list will stop
post-processing, so optionally suppressing further steps can easily be
facilitated by clearing an entry in the list.

A possible application, as provided in the example implementation,
handles a special field within the symbol maps to hold a checksum of
the data for verification.  The stored CRC value should match the
checksum calculated over the whole blob data, except for the checksum
field itself.  The example showcases both a verification and an update
step to adjust the checksum field, resulting in an image which
includes basic error detection against data corruption.


Examples
--------

Below are some command lines examples demonstrating most of
*elf-mangle*'s functions.

Generate blob `out.bin` from `in_map.elf` with default contents:

	elf-mangle in_map.elf -o out.bin -O raw

Copy values from blob `in.hex` to `out.hex` with layout of
`out_map.elf`, but override `nvm_unique` field with given hex data:

	elf-mangle in_map.elf out_map.elf -i in.hex -D nvm_unique=1a0002 -o out.hex

Print field values from blob `in.hex` according to layout of
`in_map.elf`:

	elf-mangle in_map.elf -i in.hex --print=pretty

Print default field values from `in_map.elf` converted to
`out_map.elf` layout:

	elf-mangle in_map.elf out_map.elf --print=pretty

Look for string structures with a minimum length of 12 bytes within
`in_map.elf`, looking at the `.data` section:

	elf-mangle in_map.elf -l12 -j .data

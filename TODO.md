
Issues list for elf-mangle
==========================

## String search without ELF map ##

The `--strings` option should be usable without an ELF symbol map,
e.g. to find which one needs to be used.

Factor out an `idc-strings` utility for supported image file formats.


## Override data from binary files ##

Symbol override data must be passed as hex bytes on the command line.

Support reading the from a binary file instead.


## Additional image formats ##

Support for input / output blobs:

* SREC
* Sparse output with IHEX / SREC, skipping uninitialized data:

		# Generate blob with only the specified override fields
		elf-mangle in.elf  -o out.bin --sparse -D field=123456


## Missing man page ##

Can this be extracted from the README Markdown?


## Diagnostic messages ##

* No more `fprintf(stderr, ...)`.
* Rework `#ifdef DEBUG` blocks to use `if (DEBUG)` and rely on
  optimizer.

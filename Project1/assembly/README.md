# Assembler
A general 2-pass assembler with an implementation of the LC3-2200a.

## Requirements
The assembler runs on any version of Python 2.6+.  A instruction set architecture definition file is required along with the assembler.  The LC3-2200a assembler definition is included.

## Sample Definitions
* [LC3-2200a (32-bit)](lc32200a.py)

## Options
The assembler contains multiple options.

`python assembler.py -h` prints:

    usage: Assembles generic ISA-defined assembly code into hex or binary.
           [-h] [-i ISA] [-v] [--hex] [-s SEPARATOR] asmfile

    positional arguments:
      asmfile               the .s file to be assembled

    optional arguments:
      -h, --help            show this help message and exit
      -i ISA, --isa ISA     define the Python ISA module to load [default: isa]
      -v, --verbose         enable verbose printing of assembler
      --hex, --logisim      assemble code into hexadecimal (Logisim-compatible)
      -s SEPARATOR, --separator SEPARATOR
                            the separator to use between instructions (accepts \s
                            for space and standard escape characters) [default:
                            \s]

## How to Use
Typical usage:

    python assembler.py <assembly_file> -i <isa_definition>

Example usage with the `lc32200a.py` definition:

    python assembler.py assembly.s -i lc32200a

To output assembled code in hexadecimal (compatible with *Logisim* images):

    python assembler.py assembly.s -i lc32200a --logisim

To separate entries by new line:

    python assembler.py assembly.s -i lc32200a --separator \n


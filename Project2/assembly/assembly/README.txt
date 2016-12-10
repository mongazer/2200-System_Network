The LC3-2200a Assembler and Simulator
===============

To aid in testing your processor, we have provided an *assembler* and a *simulator* for the LC3-2200a architecture.  The assembler supports converting text `.s` files into either binary (for the simulator) or hexadecimal (for pasting into Logisim) formats.

Requirements
-----------

The assembler and simulator run on any version of Python 2.6+.  An instruction set architecture definition file is required along with the assembler.  The LC3-2200a assembler definition is included.

Included Files
-----------

* `assembler.py`: the main assembler program
* `lc32200b.py`: the LC3-2200a assembler definition

Using the Assembler
-----------

### Assemble

Example usage to assemble `test.s`:

    python assembler.py -i lc32200b test.s


### Defining Delay Slots

Depending on the stage at which branches (typically either the ID/RR or EX stages), you may need more than 1 delay slot (default is 1). For example, to compile with 2 delay slots:

    python assembler.py -i lc32200b --params delay_slots=2 test.s


### Assemble for Logisim

To output assembled code in hexadecimal (compatible with *Logisim* images):

    python assembler.py -i lc32200b --logisim test.s

You can then open the resulting `test.hex` file in your favorite text editor.  In Logisim, right-click on your RAM, select **Edit Contents...**, and then copy-and-paste the contents of the hex file into the window.

Do not use the Open or Save buttons in Logisim, as it will not recognize the format.

Assembler Pseudo-ops
-----------

In addition to the syntax described in the LC3-2200a ISA reference, the assembler supports the following *pseudo-instructions*:

* `.fill`: fills a word of memory with the literal value provided

For example, `mynumber: .fill 0x500` places `0x500` at the memory location labeled by `mynumber`.

* `.word`: an alias for `.fill`
*  `RET`: an alias for `JALR $ra, $zero`


#!/bin/bash

if [ -z "$1" ] || [ "$1" == "-?" ] || [ "$1" == "--help" ]; then
	echo "USAGE: $0 PROGRAM.BIN"
	echo
	echo "Script to convert a binary image to a C-style array"
	echo "Used to build the static_rom object in simulator.c"

	exit
fi

hex=$(hexdump $1 -v -e '/4 "%X\n"' | sed s/^/0x/ | tr '\n' ',')
size=$(hexdump $1 -v -e '/4 "%X\n"' | sed s/^/0x/ | wc -l)

echo ""
echo "*** For header file: ***"
echo "// bintoarray.sh; \"$(basename $1)\""
echo "#include <stdint.h>"
echo "#define STATIC_ROM_NUM_BYTES ($size * 4)"
echo "uint32_t static_rom[$size];"

echo ""
echo "*** For source file: ***"
echo "// bintoarray.sh; \"$(basename $1)\""
echo "#include <stdint.h>"
echo "uint32_t static_rom[$size] = {$hex};"

echo ""

#!/bin/bash

hexdump $1 -v -e '/4 "%X\n"' | sed s/^/0x/ | tr '\n' ','
echo ""
hexdump $1 -v -e '/4 "%X\n"' | sed s/^/0x/ | wc -l

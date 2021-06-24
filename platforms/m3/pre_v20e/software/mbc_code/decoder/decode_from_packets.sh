#!/bin/bash

# Takes in raw packets to be decoded.
# Note: the packets need to be set up so that there are no packets from previous radio-out from the same system
# Usage: ./decode_from_packets.sh [CHIP_ID] [input file (ah_100.csv)]
# Output: organizer_input.csv, decoder_input.csv, light.csv, temp.csv in the same folder as the input file

BASH_DIR="$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)"
INPUT_DIR="$(cd "$(dirname "$2")" && pwd)"
# Extract raw packets and remove CRC bits
cut -d ',' -f 6 < "$2" | grep "0x" | sed -e 's/0x....//g' > "$INPUT_DIR/organizer_input.csv"

# Run packet organizer; this generates the decoder input
"$BASH_DIR/packet_organizer.exe" $1 "$INPUT_DIR/organizer_input.csv" "$INPUT_DIR/decoder_input.csv" > "$INPUT_DIR/organizer.log"

# Run decoder
"$BASH_DIR/decoder.exe" "$INPUT_DIR/decoder_input.csv" "$INPUT_DIR/date.log" "$INPUT_DIR/time.log" "$INPUT_DIR/light.csv" "$INPUT_DIR/temp.csv" ',' > "$INPUT_DIR/decoder.log"


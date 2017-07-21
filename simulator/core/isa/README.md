ISA Configuration
=================

Files in this folder are solely responsible for instruction _decoding_, the
implementation for all operations is in [core/operations](../operations).

Broadly speaking, the ARM Thumb ISA is heirarchical. There set of encodings
that all cores support (referred to in the ARM ARM as "All versions of the
Thumb instruction set"), here contained in [arm-thumb](arm-thumb/). Then the
Cortex-M0 adds support for [ARMv6-M](arm-v6-m) instructions, the Cortex-M3 adds
[ARMv7-M](arm-v7-m) support, and the Cortex-M4 adds [ARMv7E-M](arm-v7e-m)
opcodes.

The simulator manages this by compiling in appropriate ISA folders for the
core that is being built, controlled by a Tupfile in each subfolder.

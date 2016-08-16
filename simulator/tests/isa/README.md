Operations Tests
================

This directory contains tests for each arm instruction. The tests are designed
to run as small, self-contained units for each instruction encoding. Each test
should begin with a global `main` label which serves as entry point. These
tests assume the following instructions are well behaved:

    * `MOVS`, `MOVW`, `MOVT`, `MVN`
    * `CMP` (all forms)
    * `B<c>`, `BX LR`

Upon completion (or failure) a test should set a return code into `R0` (e.g.
`MOVS R0, #<imm>`) and return with `BX LR`. The return code `0` is used to
indicate success, non-zero for failures. Tests are encouraged to use a wide
range of error codes for fine-grained error reporting.

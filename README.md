```
         MMM       MMM
         MMM       MMM
          M M     M M
          M  M   M  M
          M   M M   M
         MMM   M   MMM
  {e/si} MMM       MMM ulator
```
[![Build Status](https://travis-ci.org/lab11/M-ulator.svg?branch=master)](https://travis-ci.org/lab11/M-ulator)

# M-ulator
This project is a highly extensible {ARM} {e,si}mulator. It is capable of both
simple simulation of various ARM cores (currently M0, M3) or in-circuit
emulation (currently the Michigan Micro Mote platform)

In addition, this project is used as a teaching tool for embedded systems
courses (currently at University of Michigan and University of Utah), both to
understand internal core design and higher-level MCU usage.

********************************************************************************

## Project Layout

  * The simulator/ directory contains the code to simulate a processor.
    The simulator is built using `tup`, a make replacement. For details, please
    see simulator/README.build and the tup homepage: http://gittup.org/tup/
    Core selection is achieved using tup variants. By default, a pipelined
    version of every supported core is created, see the simulator/configs/
    directory for other options.
    * _The simulator is licensed under the GPLv3_

  * The platforms directory contains additional support for the various cores.
    In particular, it contains software desgined to run on each of the unique
    cores supported by the simulator. In addition, the M3 platform contains some
    tools for programming M3 chips and in-circuit emulation.
    * _The platforms are licensed under the MIT/XWin license_

  * The docs/ directories hold the course materials and some amount of dated
    information on the simulator design. It is not particularly useful currently
    * _Documentation, course materials licensed under the MIT/Xwin license_

********************************************************************************

## Known Issues

  * There is little documentation currently available. Hopefully things are
    mostly intuitive / have good error messages. Github issues are appreciated
    if there are any particular pain points that need addressing.
  * Interrupts may be unreliable -- the code path is not heavily tested, though
    no known bugs currently exist.
  * Course documentation is out of date, does not reflect current projects
      - Or even all of the original projects for that matter

## Future Features / Goals

  * General clean-up of configuration options
  * Larger / more interesting peripheral library
  * Better gdb / recovery semantics
      - This likely correlates with BusFault-style work
      - This is why there is a temporary imbalance in memory semantics,
        supporting the try_read_byte function for gdb

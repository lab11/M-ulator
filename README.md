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

  * The platforms directory contains additional support for the various cores.
    In particular, it contains software desgined to run on each of the unique
    cores supported by the simulator. In addition, the M3 platform contains some
    tools for programming M3 chips and in-circuit emulation.

  * The docs/ directories hold the course materials and some amount of dated
    information on the simulator design. It is not particularly useful currently

********************************************************************************


License
-------

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or
  http://www.apache.org/licenses/LICENSE-2.0)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or
  http://opensource.org/licenses/MIT)

at your option.

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall
be dual licensed as above, without any additional terms or conditions.

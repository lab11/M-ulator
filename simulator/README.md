Building the simulator
======================

Requirements
------------

The simulator embeds Python. This requries that you have a working Python
install (you probably have this) and the Python development headers (you
probably don't have this). They are easy to install:

 * __debian-based__: sudo apt-get install python-dev
 * __rpm-based__: sudo yum install python-devel

The simulator does not use a traditional automake system. It was simply too
hard to maintain, especially with the compile-time core selection. Please read
the next section for a basic primer on tup.

Tup 101
-------

The simulator uses `tup` as a build engine. For instructions on installing tup,
please see the tup homepage: http://gittup.org/tup/

Once you have tup installed, the first step is to initialize tup in the current
working directory:

    $ /M-ulator/simulator > tup init

You then need to select the "variants" you would like to build. A list of
predefined variants is found in the `configs/` directory. For example, to build
a simulator for the Michigan Micro Mote CTLv3 CPU chip:

    $ /M-ulator/simulator > tup variant configs/373-release

This will create the directory `build-m3_ctl_v3-release/` where the built files
will be placed. To build the simulator, simply run:

    $ /M-ulator/simulator > tup upd

If you make any modifications to the simulator itself, you only need to run the
update command (`tup upd`) to rebuild the simulator.

Once you have the simulator built, you can run your programs on it, e.g.:

    $ /M-ulator/simulator > build-373-release/simulator -f ../platforms/373/software/echo_str.bin


# Running the simulator

```
USAGE: ./simulator [OPTS]

-- DEBUGGING -------------------------------------------------------------------
	-g, --gdb [port]
		Act as a remote target for gdb to debug. The optional port
		argument specifies the port to listen on, otherwise a random
		port will be selected.
	-c, --dumpatpc PC_IN_HEX
		Execute until PC reaches the given value. The simulator will
		pause at the given PC and prompt for a new pause PC
	-y, --dumpatcycle N
		Execute until cycle N and print the current machine state.
		You will be prompted for a new N at the pause
	-a, --dumpallcycles
		Prints the machine state every cycle -- This is a lot of text
	-p, --printcycles
		Prints cycle count and inst to execute every cycle (before exec)
		Conceptually, this executes between the intstruction fetch and
		decode stage, so the PC is already advanced
	-d, --decompile
		Print real-time "decompilation" of instructions as they are
		executed. Values of all instruction arguments before execution
		are printed in ()'s. Removing these (e.g. sed 's/([^)]*)//g')
		should generate a legal, exectuable stream of assembly
		(with obvious caveats of branches, pc-relative ldr/str, etc)
	-m, --memory-trace
		Print all memory accesses as they are executed.
	-s, --speed SPEED
		Specify a clock speed to run at. The units are assumed to be kHz
		unless otherwise specified. A warning will be printed if the
		simulator cannot keep up the requested speed. Setting speed
		to 0 (default) will run unthrottled.
	-e, --raiseonerror
		Raises a SIGTRAP for gdb on errors before dying
		(Useful for debugging with gdb)
-- RUN TIME --------------------------------------------------------------------
	-r, --returnr0
		Sets simulator binary return code to the return
		code of the executed program on simulator exit
	-l, --limit
		Limit CPU execution to N cycles, returns failure if hit
		(useful for catching runaway test cases)
	-T, --no-terminate
		Do not terminate when code branches to self (run forever)
	--rzwi-memory
		Treat accesses to unknown memory addresses as 'read zero,
		write ignore'. Can be useful for partially implemented cores
	-f, --flash FILE
		Flash FILE into ROM before executing
		(this file is likely somthing.bin)
	--usetestflash
		Flash the simulator with a built-in test program before running
		Conflicts with -f. The test flash program is:
			echo.bin: Listens for a character on the polling UART
			          device and echoes it back
```

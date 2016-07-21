#!/usr/bin/env python

# Mulator - An extensible {ARM} {e,si}mulator
# Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
#
# This file is part of Mulator.
#
# Mulator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Mulator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Mulator.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys

class Error(Exception):
	pass

class ParseError(Error):
	def __init__(self, e, msg=None):
		self.f, self.lineno, self.line = e.cur()
		self.msg = msg
	def __str__(self):
		print ": " + self.msg
		print
		print self.f + " line " + str(self.lineno) + ":"
		print ">>>" + self.line[:-1] + "<<<"

class EnumerableFile(enumerate):
	def __new__(self, filename):
		self.filename = filename
		return super(EnumerableFile, self).__new__(self, open(self.filename))
	def next(self):
		self.c = super(EnumerableFile, self).next()
		return self.c[1]
	def cur(self):
		return self.filename, self.c[0], self.c[1]

exceptions = {}

e = EnumerableFile("exceptions")
try:
	while True:
		# Once we've read one line, we know there's a block to parse
		idx = int(e.next().strip())
		clean = False
		module = e.next().strip()
		register = e.next().strip()
		description = e.next().strip()
		while description.count('"""') != 2:
			description += e.next()
		description = description.strip()
		func = e.next()
		if func.count('{') < 1:
			raise ParseError(e, "Expected { to begin function. Got: >>>" + func + "<<<")
		while func.count('{') != func.count('}'):
			func += e.next()
		func = func[func.find('{'):]
		empty = e.next()
		if '\n' != empty:
			raise ParseError(e, "Expected empty line. Got: >>>" + empty + "<<<")
		clean = True

		exceptions[idx] = (module,register,description,func)
except StopIteration:
	if not clean:
		raise ParseError(e, "Malformed exceptions file")

h = open("ppb.h", "w")
c = open("ppb.c", "w")

preamble = "/* THIS FILE AUTOMATICALLY GENERATED -- DO NOT EDIT */\n\n"
h.write(preamble)
c.write(preamble)

h.write('#ifndef PPB_H\n#define PPB_H\n\n')
h.write('#include "core/common.h"\n\n')
h.write('#ifndef PP_STRING\n#define PP_STRING "PPB"\n')
h.write('#include "core/pretty_print.h"\n#endif\n\n')

h.write('#define REGISTERS_BOT 0xE0000000\n')
h.write('#define REGISTERS_TOP 0xF0000000\n')
h.write('\n')

h.write('\n')

c.write('#include "ppb.h"\n')
c.write('#include "core/state_sync.h"\n')
c.write('#include "cpu/core.h"\n')
c.write('\n')

# Hack :(
c.write('#define STAGE UNK\n\n')

reset = "static void ppb_reset(void) {\n"
reset_funcs = "// Support functions for reset case\n"
reset_funcs += "// Contents generated from file 'exceptions'\n\n"
storage = "// Registers gotta live somewhere...\n"
reg_read =  "static bool ppb_read (uint32_t addr, uint32_t *val, bool debugger __attribute__ ((unused)) ) {\nswitch(addr){\n"
reg_write = "static void ppb_write(uint32_t addr, uint32_t val,  bool debugger __attribute__ ((unused)) ) {\nswitch(addr){\n"

unpredictable_func = '\
{\n\
\tWARN("Unpredictable register reset is NOP\\n");\n\
}\n'

for f in sys.argv[1:]:
	# It's a for loop! (ish)
	e = EnumerableFile(f)
	while True:
		try:
			line = e.next()
		except StopIteration:
			break

		if line[0] in '#\n':
			continue
		elif line[0] == '\t':
			try:
				bits,name = line.split()
			except ValueError:
				raise ParseError(e, "Malformatted bit-specifier line")

			if name == "RESERVED":
				continue

			bits = bits[:-1][1:]
			if ":" in bits:
				high,low = bits.split(':')
				mask = 0
				for i in range(int(low), int(high)+1):
					mask |= (1 << i)
			else:
				mask = (1 << int(bits))
			h.write("#define %s_%s_MASK 0x%08x\n" % (reg, name, mask))
			continue
		else:
			try:
				read,write,addr,init,reg = line.split()
			except ValueError:
				raise ParseError(e, "Malformatted line")

		read = read.lower()
		write = write.lower()
		addr = addr.lower()

		# normalize addr to "0x%8"
		if len(addr) is 10:
			if addr[:2] != "0x":
				raise ParseError(e, "Illegal register address: " + addr)
		if len(addr) is 8:
			a = int(addr, 16)
			addr = "0x" + str(a)

		# Validate addr is sane (ARM register map is top of addr space)
		if int(addr,16) < 0xE0000000 or int(addr,16) >= 0xF0000000:
			raise ParseError(e, "Illegal addr: " + addr)

		# build all the #defines for the header
		h.write("#define " + reg + " " + addr + "\n")

		# get some storge, variable name omits leading 0 in 0xADDR
		if read[0] == 'a' or write[0] == 'a':
			if read[0] not in ('a', '-') or write[0] not in ('a', '-'):
				raise ParseError(e, "Only read or write aliased?")
			# We still define an object for this register so the compiler
			# will alert us to any namespace conflicts; it will also
			# optimize out the unused space
			storage += "static uint32_t " + addr[1:] + " __attribute__ ((unused));\n"
		else:
			storage += "static uint32_t " + addr[1:] + ";\n"

		# generate all reset (init) code
		if "XXX" == init[0:3]:
			# Handle exception
			f_name = addr[1:] + "_reset()"
			reset_funcs += "static void " + f_name[:-1] + "void)"
			try:
				idx = int(init[4:])
				exception = exceptions[idx]
				reset_funcs += "\n// Exception Idx: "+str(idx)+"\n"
				reset_funcs += "/*\n" + exception[2] + "\n*/\n"
				reset_funcs += exception[3].replace("XXX", addr[1:]) + '\n'
			except ValueError:
				reset_funcs += "\n// Bad exception: "+init+"\n"
				reset_funcs += unpredictable_func + '\n'
			reset += "\t" + f_name + ";\n"
		elif init.count("'"):
			# Reset vector of the form N'bnnnn
			l = int(init[:init.find("'")])
			base = init[init.find("'")+1:init.find("'")+2]
			bitstring = init[init.find("'")+2:]

			if l != len(bitstring):
				raise ParseError(e, "Malformed bitstring: " + bitstring);

			# The generated code will not look pretty... it reads
			# the current value, then forces each bit individually
			reset += "\tSW(&" + addr[1:] + ", "
			reset += "SR(&" + addr[1:] + ")"
			if base in ('b', 'B'):
				for i in range(len(bitstring)):
					bit = bitstring[i]
					if bit is '1':
						reset += '|(1U<<'+str(i)+')'
					elif bit is '0':
						reset += '&~(1U<<'+str(i)+')'
					elif bit is 'x':
						pass
					else:
						raise ParseError(e, 'Bad token ' + bit + ' in binary bitstring')
			else:
				raise ParseError(e, "Bitstring of unknown base: " + init)
			reset += ');\n'
		elif init == '-':
			# NOP for reset on this register (usu write-only reg's)
			pass
		else:
			reset += "\tSW(&" + addr[1:] + ", " + init + ");\n"

		# Generate read cases
		reg_read += "case " + addr + ":\n"
		if read == 'r':
			reg_read += "\t*val = SR(&" + addr[1:] + ");\n"
			reg_read += "\treturn true;\n"
		elif read == '-':
			reg_read += "\tCORE_ERR_write_only(addr);\n"
		elif read[0] == 'a':
			# This register is aliased to another
			reg_read += "\t*val = SR(&" + read[2:] + ");\n"
			reg_read += "\treturn true;\n"
			if init != '-':
				raise ParseError(e, "Aliased register cannot define a reset ("+init+")")
		else:
			raise ParseError(e, "Unknown read token: " + read)

		# Generate write cases
		reg_write += "case " + addr + ":\n"
		if write == 'w':
			reg_write += "\treturn SW(&" + addr[1:] + ", val);\n"
		elif write == '-':
			reg_write += "\tCORE_ERR_read_only(addr);\n"
		elif write == 'c':
			# Write Clear: Any write writes 0
			reg_write += "\tif (val != 0) WARN("
			reg_write += '"Non-zero write to write-clear register\\n"'
			reg_write += ");\n"
			reg_write += "\treturn SW(&" + addr[1:] + ", 0);\n"
		elif write[0] == 'a':
			# This register is aliased to another
			reg_write += "\treturn SW(&" + read[2:] + ", val);\n"
			if init != '-':
				raise ParseError(e, "Aliased register cannot define a reset ("+init+")")
		else:
			raise ParseError(e, "Unknown write token: " + write)


reset += "} // End ppb_reset\n\n"
reset_funcs += "// End supporting reset funcs\n\n"
storage += "// End storage\n\n"
reg_read += "default:\n\tCORE_ERR_invalid_addr(false, addr);\n}\n}\n"
reg_write += "default:\n\tCORE_ERR_invalid_addr(true, addr);\n}\n}\n"

h.write('\n#endif // PPB_H\n');

c.write(storage)
c.write(reset_funcs)
c.write(reset)
c.write(reg_read)
c.write(reg_write)

c.write('''
__attribute__ ((constructor))
void register_reset_ppb(void) {
    register_reset(ppb_reset);
}

__attribute__ ((constructor))
void register_memmap_ppb(void) {
    union memmap_fn mem_fn;

    mem_fn.R_fn32 = ppb_read;
    register_memmap("PPB", false, 4, mem_fn, REGISTERS_BOT, REGISTERS_TOP);

    mem_fn.W_fn32 = ppb_write;
    register_memmap("PPB", true, 4, mem_fn, REGISTERS_BOT, REGISTERS_TOP);
}
''')

h.close()
c.close()

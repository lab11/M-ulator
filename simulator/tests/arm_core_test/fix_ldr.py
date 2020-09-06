#!/usr/bin/env python

import sys

IN_FILE='alu.long_ldr'
OUT_FILE='alu.s'

def line_generator():
    for line in open(IN_FILE):
        yield line

counter = 1
o = open(OUT_FILE, 'w')
g = line_generator()
for line in g:
    line = line.rstrip() + '\n'

    # Re-write the branch to failure logic
    if -1 != line.find("# Test Complete"):
        o.write(line)

        line = g.next() # B ..._rtn_lbl
        line = g.next() # '\n'
        line = g.next() # # Failure
        fail_lbl = g.next() # ..._led_lbl:
        line = g.next() # ldr r0, clk_freq / 5
        if line.find('CLK_FREQ') == -1:
            print "ERR corrected branch logic. Expected clk line, got:"
            print ">>>" + line + "<<<"
            sys.exit(1)
        line = g.next() # ldr r1, ledLoop
        if line.find('LDR R1, ') == -1:
            print "ERR corrected branch logic. Expected ldr ledLoop line, got:"
            print ">>>" + line + "<<<"
            sys.exit(1)
        line = g.next() # bx r1
        if line.strip() != 'BX  R1':
            print "ERR corrected branch logic. Expected bx r1, got:"
            print ">>>" + line.strip() + "<<<"
            sys.exit(1)
        line = g.next() # '\n'
        if line.strip() != '':
            print "ERR corrected branch logic. Expected blank line, got:"
            print ">>>" + line.strip() + "<<<"
            sys.exit(1)
        line = g.next() # # Return from function
        line = g.next() # ...rtn_lbl:
        line = g.next() # bx lr
        if line.strip() != 'BX LR':
            print "ERR corrected branch logic. Expected bx lr, got:"
            print ">>>" + line.strip() + "<<<"
            sys.exit(1)

        o.write('    MOVW R0, 0\n')
        o.write('    BX LR\n')
        o.write('\n')

        o.write(fail_lbl.rstrip() + '\n')
        o.write('    MOVW R0, ' + str(counter) + '\n')
        counter += 1
        o.write('    BX LR\n')
        continue

    if -1 != line.find('_led_lbl:'):
        print "ERR, missed branch correction, line:"
        print ">>>" + line + "<<<"
        sys.exit(1)

    # Re-write the end test sequences
    if -1 != line.find('_instruction_test:'):
        o.write(line)

        for line in g:
            if line.strip() == 'BX LR':
                o.write('    BX LR\n')
                break
            o.write(line.rstrip() + '\n')
            o.write('    CMP R0, 0\n')
            o.write('    BNE alu_error\n')
        continue

    # Ignore other comments
    try:
        if line.lstrip()[0] == '#':
            o.write(line)
            continue
    except IndexError:
        o.write(line)
        continue

    try:
        line.index('LDR')
    except ValueError:
        o.write(line)
        continue

    print line.rstrip()
    l = line.split()
    try:
        val = int(l[-1], 16)
    except ValueError:
        # Target is a label or variable
        o.write(line)
        continue
    if (val < 4096) and (val > 0):
        o.write('    MOVW %s 0x%x\n' % (l[1], val))
        continue

    if (val > 2**32):
        raise ValueError
    #if (val < 0):
    #    val = 2**32 + val

    bot = val & 0xffff
    top = (val >> 16) & 0xffff

    o.write('    MOVW %s 0x%x\n' % (l[1], bot))
    o.write('    MOVT %s 0x%x\n' % (l[1], top))

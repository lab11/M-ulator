Notes for future users.

20190415 Kyojin - I changed the M0 processor's initial stack pointer to 0x15e0 (from original 0x2000) by changing the entry in ./libs/vectors.s.
--> This is to get around problem caused by EP trying to write VIM program to PRE sram in programs (VIMstack_loadprog_*.c) starting from address "1400*4" - 600*4 byte length program
--> When stack pointer starts from 0x2000, it mucks with VIM program and goes into infinite loop always making 'correctness test' in VIMstack_loadprog_*.c to go false.
--> DO NOT ever push ./libs to GIT because it is not standard.
--> Hope 0x15e0 is enough memory space for VIMstack program to work.
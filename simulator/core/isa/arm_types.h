/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef __ARM_TYPES_H
#define __ARM_TYPES_H

#define INST_SET_ARM		0x0
#define INST_SET_THUMB		0x1
#define INST_SET_JAZELLE	0x2
#define INST_SET_THUMBEE	0x3

#define SRType_LSL	0x0
#define SRType_LSR	0x1
#define SRType_ASR	0x2
#define SRType_RRX	0x3
#define SRType_ROR	0x3

enum SRType {
	LSL,	// match
	LSR,	// match
	ASR,	// match
	RRX,	// match
	ROR,	// +1
};

#endif // __ARM_TYPES_H


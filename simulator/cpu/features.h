/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef FEATURES_H
#define FEATURES_H

inline __attribute__ ((always_inline))
bool HaveDSPExt(void) {
	return false;
}

inline __attribute__ ((always_inline))
bool HaveFPExt(void) {
	return false;
}

#endif // FEATURES_H


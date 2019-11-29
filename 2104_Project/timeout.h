/*
 * timeout.h
 *
 *  Created on: Nov 21, 2019
 *      Author: dxong
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

// initialize registers related to timeout module
void initTimeout(unsigned int millisecondCycles);
// start timeout with a delay then call the given function
// returns an id
int setTimeout(unsigned int ms, void (*callback)());
// clear the timeout given an id
void clearTimeout(int id);
#endif /* TIMEOUT_H_ */

/*
 * timeout.h
 *
 *  Created on: Nov 21, 2019
 *      Author: dxong
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_


void initTimeout(unsigned int millisecondCycles);
int setTimeout(unsigned int ms, void (*callback)());
void clearTimeout(int id);
#endif /* TIMEOUT_H_ */

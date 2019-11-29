/*
 * ultrasonic.h
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <stdio.h>
#include "msp.h"

#define TB BIT0
#define UB BIT1

// initialize registers related to the module
void initUltrasonic();
// trigger the call to read from ultrasonic sensor 2
void readUltrasonic2(void (*callback)(float));
// trigger the call to read from ultrasonic sensor 1
void readUltrasonic1(void (*callback)(float));


#endif /* ULTRASONIC_H_ */

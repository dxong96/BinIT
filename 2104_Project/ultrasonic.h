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

#define TB BIT4
#define UB BIT5


void trigger(void);
void read_ultrasonic(void);


#endif /* ULTRASONIC_H_ */
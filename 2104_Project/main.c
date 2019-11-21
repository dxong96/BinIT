#include "msp.h"
#include "temp_sensor.h"
#include "ultrasonic.h"
#include "lcd16.h"
#include "general_clock.h"
#include <stdio.h>

/**
 * main.c
 */

void ultrasonic1Handler(float distance) {
    printf("ultrasonic 1 distance %f\n", distance);
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

//	lcdinit();
////    prints("ICT2104 SIT 2019");
//    gotoXy(0,1);
//    prints("Buttons ISR Demo");

//	float result[2];
//	readTempSensor(result);
//
//	printf("celcius %f, humidity: %f", result[0], result[1]);
	initializeGeneralClock();
	init_ultrasonic();
	while (1) {
        read_ultrasonic2(ultrasonic1Handler);
        __delay_cycles(3000000);
	}

	return 0;
}

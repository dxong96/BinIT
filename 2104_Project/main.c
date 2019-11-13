#include "msp.h"
#include <stdio.h>
#include "temp_sensor.h"
#include "ultrasonic.h"
#include "lcd16.h"

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	 lcdinit();
    prints("ICT2104 SIT 2019");
    gotoXy(0,1);
    prints("Buttons ISR Demo");

//	float result[2];
//	readTempSensor(result);
//
//	printf("celcius %f, humidity: %f", result[0], result[1]);


    read_ultrasonic();

	return 0;
}

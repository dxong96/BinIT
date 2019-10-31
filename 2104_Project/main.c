#include "msp.h"
#include <stdio.h>
#include "temp_sensor.h"

/**
 * main.c
 */
void main(void)
{
//	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	float result[2];
	readTempSensor(result);

	printf("celcius %f, humidity: %f", result[0], result[1]);

	return 0;
}

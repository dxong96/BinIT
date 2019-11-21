#include "msp.h"
#include "temp_sensor.h"
#include "ultrasonic.h"
#include "lcd16.h"
#include "general_clock.h"
#include "timeout.h"
#include "wifi.h"
#include <stdio.h>

/**
 * main.c
 */

void ultrasonic1Handler(float distance) {
    printf("ultrasonic 1 distance %f\n", distance);
}

void ultrasonic2Handler(float distance) {
    printf("ultrasonic 2 distance %f\n", distance);
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

	initTimeout(3000);
	initializeGeneralClock();
	init_ultrasonic();

	// ultra sonic test
//	while (1) {
//        read_ultrasonic2(ultrasonic1Handler);
//        __delay_cycles(3000000);
//	}

	// wifi test
	initWifi();

    exitTransferMode();

    if(!isReady()) {
        printf("Wifi not ready exiting.\n");
        return 1;
    }

    if (!setWifiMode()) {
        printf("Fail to set wifi mode\n");
        return 1;
    }

    if (!connectToAP("Dx phone", "97875031")) {
        printf("Fail to connect to access point\n");
        return 1;
    }

    getRequest("google.com", "/");
    printf("http result\n%s\n", getReply());
    while (1) {
        __no_operation();
    }

	return 0;
}

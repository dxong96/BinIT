#include "msp.h"
#include "temp_sensor.h"
#include "ultrasonic.h"
#include "lcd16.h"
#include "general_clock.h"
#include "timeout.h"
#include "wifi.h"
#include "servo_m.h"
#include <stdio.h>

#define BIN_HEIGHT 32
#define BIN_CHECK_HEIGHT 24
#define MAIN_LOOP_INTERVAL 1000 // every 1s
#define VOLUME_LOOP_INTERVAL 60000 // every one minute
/**
 * main.c
 */

void volumeCheckLoop();
void binCapLoop();
void ultrasonic1Handler(float distance) {
    printf("ultrasonic 1 distance %f\n", distance);
    if (distance < 30) {
        goToPosition0();
    } else {
        goToPosition180();
    }
}

char lcdStrBuffer[100];
void ultrasonic2Handler(float distance) {
    float percentFull = (BIN_HEIGHT - distance) / BIN_CHECK_HEIGHT * 100;
    if (percentFull < 0) {
        percentFull = 0;
    }

    float results[2];
    readTempSensor(results);

    // show lcd
    gotoXy(0, 0);
    sprintf(lcdStrBuffer, "Temp: %.1f C", results[0]);
    prints(lcdStrBuffer);
    gotoXy(0,1);
    sprintf(lcdStrBuffer, "%.1f %% full", percentFull);
    prints(lcdStrBuffer);

    // call wifi function
    setTimeout(VOLUME_LOOP_INTERVAL, volumeCheckLoop);
}

void binCapLoop() {
    printf("binCapLoop\n");
    read_ultrasonic1(ultrasonic1Handler);
    setTimeout(MAIN_LOOP_INTERVAL, binCapLoop);
}

void volumeCheckLoop() {
    printf("volumeCheckLoop\n");
    read_ultrasonic2(ultrasonic2Handler);
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	P6DIR = BIT0 + BIT1;
    P4DIR = BIT0 + BIT2 + BIT3 + BIT4 + BIT5;

    P6OUT = 0;
    P4OUT = 0;
	lcdinit();


//	float result[2];
//	readTempSensor(result);
//
//	printf("celcius %f, humidity: %f", result[0], result[1]);

	initTimeout(3000);
	initializeGeneralClock();
	init_ultrasonic();
	initServo();
	goToPosition180();


    volumeCheckLoop();

	// ultra sonic test
//	while (1) {
//        read_ultrasonic2(ultrasonic1Handler);
//        __delay_cycles(3000000);
//	}


	// wifi test
//	initWifi();
//
//    exitTransferMode();
//
//    if(!isReady()) {
//        printf("Wifi not ready exiting.\n");
//        return 1;
//    }
//
//    if (!setWifiMode()) {
//        printf("Fail to set wifi mode\n");
//        return 1;
//    }
//
//    if (!connectToAP("Dx phone", "97875031")) {
//        printf("Fail to connect to access point\n");
//        return 1;
//    }
//
//    getRequest("google.com", "/");
//    printf("http result\n%s\n", getReply());
//    while (1) {
//        __no_operation();
//    }

	setTimeout(1000, binCapLoop);
	setTimeout(5000, volumeCheckLoop);

	 SCB->SCR|= SCB_SCR_SLEEPDEEP_Msk;// Set SLEEPDEEP
	 SCB->SCR|= SCB_SCR_SLEEPONEXIT_Msk;// Set SLEEPONEXIT
	 __DSB();// Ensures SLEEPONEXIT is set
	 __WFI();
}

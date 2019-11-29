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
int callServer = 0;
/**
 * main.c
 */

// forward defined functions
void volumeCheckLoop();
void binCapLoop();

// the ultrasonic that determines whether to open the bin cap
void ultrasonic1Handler(float distance) {
    printf("ultrasonic 1 distance %f\n", distance);
    // when distance is less than 30cm open the bin cap
    if (distance < 30) {
        goToPosition0();
    } else {
        goToPosition180();
    }
}

char lcdStrBuffer[100]; // buffer to generate string for lcd display
// ultrasonic to check the fullness of the bin
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

    printf("%.1f\n", distance);

    callServer = 1;
    sprintf(lcdStrBuffer, "/temp_fullness?temperature=%.1f&fullness=%.1f", results[0], percentFull);

    // call wifi function
    // we did not call the function here due to deadlock if we do call it.
//    getRequest("192.168.43.58", lcdStrBuffer, 3000);

    setTimeout(VOLUME_LOOP_INTERVAL, volumeCheckLoop);
}

void binCapLoop() {
    printf("binCapLoop\n");
    readUltrasonic1(ultrasonic1Handler);
    setTimeout(MAIN_LOOP_INTERVAL, binCapLoop);
}

void volumeCheckLoop() {
    printf("volumeCheckLoop\n");
    readUltrasonic2(ultrasonic2Handler);
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	P6DIR = BIT0 + BIT1;
    P4DIR = BIT0 + BIT2 + BIT3 + BIT4 + BIT5;

    P6OUT = 0;
    P4OUT = 0;
	lcdinit();
	initTimeout(3000);
	initializeGeneralClock();
	initUltrasonic();
	initServo();
	goToPosition180();


	// wifi test
	initWifi();

//
    exitTransferMode();

    if(!isReady()) {
        printf("Wifi not ready exiting.\n");
        return 1;
    }

    if (!setWifiMode()) {
        printf("Fail to set wifi mode\n");
        return 1;
    }

    if (!connectToAP("OnePlus 5T", "pangjunrong")) {
        printf("Fail to connect to access point\n");
        return 1;
    }
    volumeCheckLoop();

	setTimeout(1000, binCapLoop);
	setTimeout(5000, volumeCheckLoop);
//
	while(1){
	    if (callServer){
	        callServer = 0;
	        getRequest("192.168.43.58", lcdStrBuffer, 3000);

	    }
	}

//   we intended to sleep the MCU and let it run only on interrupt but due to the deadlock we have no choice but to write a while loop
//	 SCB->SCR|= SCB_SCR_SLEEPDEEP_Msk;// Set SLEEPDEEP
//	 SCB->SCR|= SCB_SCR_SLEEPONEXIT_Msk;// Set SLEEPONEXIT
//	 __DSB();// Ensures SLEEPONEXIT is set
//	 __WFI();
}

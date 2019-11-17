#include "msp.h"
#include "wifi.h"
#include <stdio.h>

/**
 * main.c
 */
int main(void)
{
    int reply = -1;

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    init();

    exitTransferMode();

    if(!isReady()) {
        printf("Wifi not ready exiting.");
        return 1;
    }

    if (!setWifiMode()) {
        printf("Fail to set wifi mode");
        return 1;
    }

    if (!connectToAP("Dx phone", "97875031")) {
        printf("Fail to connect to access point");
        return 1;
    }

    getRequest("google.com", "/");
    printf("http result\n%s\n", getReply());
    while (1) {
        __no_operation();
    }
}


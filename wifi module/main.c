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

    sendString("+++");
    __delay_cycles(3000000);

    sendString("AT\r\n");
    waitForReply(1, "OK");

    sendString("AT+CWMODE_CUR=1\r\n");
    waitForReply(1, "OK");

    sendString("AT+CWJAP_CUR=\"Dx phone\",\"97875031\"\r\n");
    reply = waitForReply(2, "OK", "FAIL");
    if (reply == 1) {
        printf("Fail to connect to AP");
        return 1;
    }

    sendString("AT+CIPSTART=\"TCP\",\"www.columbia.edu\",80\r\n");
    waitForReply(3, "OK", "ERROR", "ALREADY CONNECTED");

    sendString("AT+CIPMODE=1\r\n");
    waitForReply(1, "OK");

    sendString("AT+CIPSEND\r\n");
    waitForReply(1, ">");
    sendString("GET /~fdc/nowax.html HTTP/1.0\r\n\r\n");
    waitForRequestReply();
    printf("http result\n%s\n", getReply());
    while (1) {
        __no_operation();
    }
}


/*
 * wifi.c
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */

#include "msp.h"
#include <string.h>
#include <stdio.h>
#include "wifi.h"
#include "timeout.h"

// receive buffer for uart
char rxBuffer[BUFFER_SIZE];
int readPointer = 0;
int writePointer = 0;

// read access variable
char replyBuffer[BUFFER_SIZE];
int timeout = 0;
int timeoutId = -1;

void timeoutCallback() {
    timeout = 1;
    printf("timeout callback\n");
}

void startTimeout(int ms) {
    stopTimeout();
    timeout = 0;
    timeoutId = setTimeout(ms, timeoutCallback);
}

void stopTimeout() {
    if (timeoutId != -1) {
        clearTimeout(timeoutId);
        timeoutId = -1;
    }
}

void sendString(const char *string){

    const char * i = string;
    while (*i) {
        UCA2TXBUF = *i;
        while (!(UCA2IFG & 0x02));
        i++;
    }

}

int bufferAvailable() {
    return readPointer < writePointer;
}


int waitForReply(int length, ...) {
    timeout = 0;
    int i = 0;
    int y = 0;
    va_list arguments;
    va_start (arguments, length);
    char * responses[5] = {};

    // copy the va args
    for (; y < length; y++) {
        responses[y] = va_arg(arguments, char *);
    }

    while (1) {
        if (timeout) {
            va_end(arguments);
            return -1;
        }
        while(bufferAvailable()) {
            // copy for reading, should be read before calling another command
            replyBuffer[i++] = rxBuffer[readPointer++];
        }
        // end with null character
        replyBuffer[i] = 0;
        for (y = 0; y < length; y++) {
            if (strstr(replyBuffer, responses[y]) != NULL) {
                va_end(arguments);
                stopTimeout();
                return y;
            }
        }
    }
}

void waitForRequestReply() {
    int i, y;
    i = 0;

    __delay_cycles(3000000); // delay 1s

    for (y = 0; y < 10; y++) {
        // delay 10ms
        __delay_cycles(30000);
        while(bufferAvailable()) {
            // copy for reading, should be read before calling another command
            replyBuffer[i++] = rxBuffer[readPointer++];
        }
        // end with null character
        replyBuffer[i] = 0;
    }
}

void initWifi() {
    // Configure UART pins
    P3SEL0 |= UART1B | UART2B;                  // set 2-UART pin as second function
    P3SEL1 &= ~(UART1B | UART2B);

    EUSCI_A2->CTLW0 |= 1;
    EUSCI_A2->MCTLW = 0;
    EUSCI_A2->CTLW0 = 0x0081;
    EUSCI_A2->BRW = 26;
    EUSCI_A2->CTLW0 &= ~1;
    EUSCI_A2->IE |= BIT0;

    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31); // Enable eUSCIA2 interrupt in NVIC module
}

char * getReply() {
    return replyBuffer;
}

void exitTransferMode() {
    sendString("+++");
    __delay_cycles(3000000);
}

int isReady() {
    sendString("AT\r\n");
    startTimeout(3000);
    int res = waitForReply(1, "OK");
    return res == 0;
}

int setWifiMode() {
    sendString("AT+CWMODE_CUR=1\r\n");
    startTimeout(15000);
    int res = waitForReply(1, "OK");
    return res == 0;
}

int connectToAP(char * ssid, char * password) {
    char buf[256];
    snprintf(buf, 256, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, password);
    sendString(buf);
    startTimeout(45000);
    int reply = waitForReply(2, "OK", "FAIL");
    if (reply == 1) {
        printf("Fail to connect to AP");
    } else if (reply == -1) {
        printf("Fail to connect to AP due to timeout");
    }
    return 1;
}

void getRequest(char * host, char * path) {
    char buf[256];
    snprintf(buf, 256, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", host);
    sendString(buf);
    waitForReply(3, "OK", "ERROR", "ALREADY CONNECTED");

    sendString("AT+CIPMODE=1\r\n");
    waitForReply(1, "OK");

    sendString("AT+CIPSEND\r\n");
    waitForReply(1, ">");
    snprintf(buf, 256, "GET %s HTTP/1.0\r\n\r\n", path);
    sendString(buf);
    waitForRequestReply();
    exitTransferMode();
}

// UART interrupt service routine
void EUSCIA2_IRQHandler(void)
{
    if (UCA2IFG & UCRXIFG)
    {
        rxBuffer[writePointer] = UCA2RXBUF;
        writePointer = (writePointer + 1) % BUFFER_SIZE;
    }
    UCA2IFG &= ~UCRXIFG;
}






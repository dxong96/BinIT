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

// receive buffer for uart
char rxBuffer[BUFFER_SIZE];
int readPointer = 0;
int writePointer = 0;

// read access variable
char replyBuffer[BUFFER_SIZE];


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
        while(bufferAvailable()) {
            // copy for reading, should be read before calling another command
            replyBuffer[i++] = rxBuffer[readPointer++];
        }
        // end with null character
        replyBuffer[i] = 0;
        for (y = 0; y < length; y++) {
            if (strstr(replyBuffer, responses[y]) != NULL) {
                va_end(arguments);
                return y;
            }
        }
    }
    return -1;
}


void wifi(){

    int reply = -1;

      // Configure UART pins
      P3SEL0 |= UART1B | UART2B;                  // set 2-UART pin as second function
      P3SEL1 &= ~(UART1B | UART2B);

      P1->SEL0 &= ~WB;
      P1->SEL1 &= ~WB;
      P1->DIR &= ~WB;

      EUSCI_A2->CTLW0 |= 1;
      EUSCI_A2->MCTLW = 0;
      EUSCI_A2->CTLW0 = 0x0081;
      EUSCI_A2->BRW = 26;
      EUSCI_A2->CTLW0 &= ~1;
      EUSCI_A2->IE |= BIT0;

      __enable_interrupt();
      NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31); // Enable eUSCIA2 interrupt in NVIC module

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

      sendString("AT+CIPSTART=\"TCP\",\"api.themoviedb.org\",80\r\n");
      waitForReply(3, "OK", "ERROR", "ALREADY CONNECTED");

      sendString("AT+CIPMODE=1\r\n");
      waitForReply(1, "OK");

      sendString("AT+CIPSEND\r\n");
      waitForReply(1, ">");
      sendString("GET /3/search/movie?api_key=f717457915ccf18636da91d9319b25b5&query=a+beautiful+mind HTTP/1.0\r\n\r\n");
      while (1) {
          __no_operation();
      }

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








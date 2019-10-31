/*
 * wifi.h
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */

#ifndef WIFI_H_
#define WIFI_H_


    #define BUFFER_SIZE 2048
    #define  WB BIT6
    #define UART1B BIT2
    #define UART2B BIT3

    void sendString(const char *string);
    int bufferAvailable(void);
    int waitForReply(int length, ...);
    void wifi(void);


#endif /* WIFI_H_ */

/*
 * wifi.h
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */

#ifndef WIFI_H_
#define WIFI_H_


    #define BUFFER_SIZE 2048
    #define UART1B BIT2
    #define UART2B BIT3

    // callback for timeout checker
    void initWifi();
    // start timer to run after maximum waiting time
    void startTimeout(int ms);
    // stop timer
    void stopTimeout();
    // send string to via uart to wifi module
    void sendString(const char *string);
    // check if there is unread data in the buffer
    int bufferAvailable(void);
    // wait for reply from wifi module, given expected responses
    int waitForReply(int length, ...);
    // wait for get request reply
    void waitForRequestReply();
    // read the data from reply
    char * getReply();
    // exit the mode to send data
    void exitTransferMode();
    // check if the wifi module is ready
    int isReady();
    // set wifi mode to station mode
    int setWifiMode();
    // connect to the specified access point
    int connectToAP(char * ssid, char * password);
    // make a get request
    void getRequest(char * host, char * path, unsigned int port);

#endif /* WIFI_H_ */

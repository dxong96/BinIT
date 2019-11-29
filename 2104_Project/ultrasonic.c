/*
 * ultrasonic.c
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */
#include "ultrasonic.h"
#include "general_clock.h"
#include <limits.h>
#include <stdio.h>

int counter1Id;
int counter2Id;

void (*ultrasonic1Callback)(float) = NULL;
void (*ultrasonic2Callback)(float) = NULL;

void initUltrasonic() {
    // ultrasonic 1
    P1->DIR |= BIT6;
    P6->DIR &= ~BIT7;

    P6->IFG &= ~BIT7;
    P6->IE |= BIT7;
    P6->IES |= BIT7;
    NVIC->ISER[1] |= 1 << (PORT6_IRQn & 31);
    counter1Id = startTracking();

    // ultrasonic 2
    P5DIR |= TB;     // trigger pin as output
    P5->DIR &= ~UB;  // set p1.5 input

    P5IFG = 0x00;      // clear flag just in case anything happened before
    P5IE |= UB;      // enable interupt on ECHO pin
    P5IES |= UB;  //falling edge
    NVIC->ISER[1] |= 1 << (PORT5_IRQn & 31);
    counter2Id = startTracking();
}

void readUltrasonic1(void (*callback)(float)){
    P1->OUT |= BIT6;
    __delay_cycles(30);
    P1->OUT &= ~BIT6;
    resetCounter(counter1Id);
    ultrasonic1Callback = callback;
}

void readUltrasonic2(void (*callback)(float)) {
    P5->OUT |= TB;
    __delay_cycles(30);
    P5->OUT &= ~TB;
    resetCounter(counter2Id);
    // set the address to the callback function
    ultrasonic2Callback = callback;
}

float calculateDistance(int counterId) {
    int loops = getLoops(counterId);
    int counter = getCounter(counterId);
    if (loops == 1) {
         counter = INT_MAX - counter;
    }
    float ms = counter / 3;
    float distance = ms / 53;
    return distance;
}

void PORT6_IRQHandler(void) {
    if (P6->IFG & BIT7)
    {
        if (ultrasonic1Callback != NULL) {
            ultrasonic1Callback(calculateDistance(counter1Id));
        }
    }
    P6->IFG &= ~BIT7;
}

void PORT5_IRQHandler(void){
    if(P5IFG & UB)  //is there interrupt pending?
    {
        // call the resulting function
        ultrasonic2Callback(calculateDistance(counter2Id));
    }
    P5IFG &= ~UB;   //clear flag
}








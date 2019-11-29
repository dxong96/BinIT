/*
 * timeout.c
 *
 *  Created on: Nov 21, 2019
 *      Author: dxong
 */
#include "msp.h"

#define TIMEOUT_SIZE 4

unsigned int durations[TIMEOUT_SIZE];
unsigned int timePassed[TIMEOUT_SIZE];
void (*callbacks[TIMEOUT_SIZE])();

void initTimeout(unsigned int millisecondCycles) {
    TIMER32_1->LOAD = millisecondCycles - 1;
    TIMER32_1->CONTROL = 0xE2; // enable; periodic mode, enable interrupt, prescale 0, 32-bit, wrapping mode
    NVIC->ISER[0] |= 1 << T32_INT1_IRQn;
}

int setTimeout(unsigned int ms, void (*callback)()) {
    int i;
    for (i = 0; i < TIMEOUT_SIZE; i++) {
        if (durations[i] == 0) {
            // set up the variables for running an event after x milliseconds
            durations[i] = ms;
            timePassed[i] = 0;
            callbacks[i] = callback;
            return i;
        }
    }
}

void clearTimeout(int id) {
    durations[id] = 0;
    timePassed[id] = 0;
}


void T32_INT1_IRQHandler(void) {
    if (TIMER32_1->RIS & 1 == 0) return;
    int i;
    for (i = 0; i < TIMEOUT_SIZE; i++) {
        if (durations[i] == 0)
            continue;

        timePassed[i]++;
        // time has reached the desired duration
        if (durations[i] == timePassed[i]) {
            clearTimeout(i);
            if (callbacks[i]) {
                callbacks[i]();
            }
            continue;
        }
    }

    TIMER32_1->INTCLR = 0;
}

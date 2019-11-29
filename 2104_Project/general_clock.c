#include "msp.h"

unsigned int counters[8];
unsigned int loops[8];
unsigned char availableCounters;
unsigned char isTracking = 0;

void initializeGeneralClock() {
    TIMER32_2->CONTROL = 0xA2; // enable; free running mode, enable interrupt, prescale 0, 32-bit, wrapping mode

    // Enable Timer interrupt on the NVIC
    // enable interrupt for timer_2
    NVIC->ISER[0] |= 1 << T32_INT2_IRQn;
}

int startTracking() {
    // check if all the counters are taken
    if (availableCounters == 0xFF) {
        return -1;
    }
    // find the available counter and return the available index
    int pos = 1;
    int index = 0;
    while ((availableCounters & pos) != 0) {
        pos <<= 1;
        index++;
    }
    return index;
}

void stopTracking(int id) {
    int pos = 1 << id;
    availableCounters &= ~pos; // free up the index
}

void resetCounter(int id) {
    counters[id] = TIMER32_2->VALUE; // reset the counter
    loops[id] = 0; // reset the number of loops
}

int getCounter(int id) {
    unsigned int timerVal = TIMER32_2->VALUE;
    return counters[id] - timerVal; // diff between the recorded counter and current counter
}

int getLoops(int id) {
    return loops[id];
}

void T32_INT2_IRQHandler(void) {
    if (TIMER32_2->RIS & 1 == 0) return;
    // when the timer loops back to 0
    int i;
    for (i = 0; i < 8; i++) {
        int pos = 1 << i;
        if ((pos & availableCounters) != 0) {
            loops[i]++;
        }
    }
}

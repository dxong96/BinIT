/*
 * config.c
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */
#include "msp.h"
#include <stdio.h>

void delayUs(int n)
{
    int i;

    TIMER32_1->LOAD = 3; /* reload with number of clocks per millisecond */
    /* no prescaler, periodic wrapping mode, disable interrupt, 32-bit timer. */
    TIMER32_1->CONTROL = 0xC2;

    for(i = 0; i < n; i++) {
        while((TIMER32_1->RIS & 1) == 0); /* wait until the RAW_IFG is set */
        TIMER32_1->INTCLR = 0;            /* clear RAW_IFG flag */
    }
}


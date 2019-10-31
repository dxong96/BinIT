/*
 * ultrasonic.c
 *
 *  Created on: 31 Oct 2019
 *      Author: markpang
 */
#include "ultrasonic.h"
#include <stdio.h>
#include "config.h"

int seconds;
int distance;
long sensor;

void trigger(void){

   P2OUT |= TB;     // Set P2.4 as HIGH
   __delay_cycles(10);   // for 10us
   P2OUT &= ~TB;    // stop pulse
   TIMER_A0->CTL |= TIMER_A_CTL_CLR; //clear the timer
}


void read_ultrasonic(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // stop watchdog timer

      /* Timer_A UpMode Configuration Parameter */
      TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK|    // SMCLK
                          TIMER_A_CTL_MC_1 |      // Up mode
                          TIMER_A_CTL_CLR |       // Clear TAR
                          TIMER_A_CTL_IE;         // Enable overflow interrupt

      P2DIR |= TB;     // trigger pin as output
      P1->DIR &= ~UB;  // set p1.5 input

      P1IFG = 0x00;      // clear flag just in case anything happened before
      P1IE |= UB;      // enable interupt on ECHO pin
      P1IES |= UB;  //falling edge
      NVIC->ISER[1] |= 1 << (PORT1_IRQn & 31);
      __enable_interrupt();  // global interrupt enable

      TA0CCR0 = 60000; //set the timer
      trigger();
      while (1) { //so that the program stop eruptly
      }

}

void PORT1_IRQHandler(void){
    if(P1IFG & UB)  //is there interrupt pending?
    {
      sensor = TA0R / 3;    //calculating ECHO lenght
      distance = sensor/148;  // converting ECHO lenght into cm
      printf("%d\n", distance);
      __delay_cycles(20 * 60 * 1000);
      trigger();
    }
      P1IFG &= ~UB;   //clear flag
}








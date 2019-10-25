/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
//#include "msp.h"

int seconds;
int distance;
long sensor;

void main(void)
{
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // stop watchdog timer

  /* Timer_A UpMode Configuration Parameter */
  TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK|    // SMCLK
                      TIMER_A_CTL_MC_1 |      // Up mode
                      TIMER_A_CTL_CLR |       // Clear TAR
                      TIMER_A_CTL_IE;         // Enable overflow interrupt

  P2DIR |= BIT4;     // trigger pin as output
  P1->DIR &= ~BIT5;  // set p1.5 input
//  P1->REN |= BIT5;
//  P1->OUT &= ~BIT5;

  P1IFG = 0x00;      // clear flag just in case anything happened before
  P1IE |= BIT5;      // enable interupt on ECHO pin
//  P1IES &= ~BIT5;    // rising edge on ECHO pin
  P1IES |= BIT5;  //falling edge
  NVIC->ISER[1] |= 1 << (PORT1_IRQn & 31);
  __enable_interrupt();                         // global interrupt enable


  TA0CCR0 = 60000;
  trigger();
  while (1) {
  }
}

void trigger() {
    P2OUT |= BIT4;     // Set P2.4 as HIGH
    __delay_cycles(10);         // for 10us
    P2OUT &= ~BIT4;    // stop pulse
    TIMER_A0->CTL |= TIMER_A_CTL_CLR; //clear the timer
}

void PORT1_IRQHandler(void){
    if(P1IFG & BIT5)  //is there interrupt pending?
    {
      sensor = TA0R / 3;    //calculating ECHO lenght
      distance = sensor/148;           // converting ECHO lenght into cm
      printf("%d\n", distance);
      __delay_cycles(20 * 60 * 1000);
      trigger();
    }
      P1IFG &= ~BIT5;             //clear flag
}

void TA0_0_IRQHandler(void){
    seconds++;
    printf("seconds: %d\n", seconds);
}

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
}/* 1 sec can do 300000 operations,every 3000 sec trigger timer 300000/3000 = 1000
    wait for a timer trigger is a 1ms for each iteration 1000 times will guarantee 1 sec*/

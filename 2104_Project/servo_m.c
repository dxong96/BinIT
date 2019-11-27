#include "servo_m.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,       // 3MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,  // divide by 1
        60000,                           // 20ms
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        4500
};

void initServo()
{
    P2->SEL0 |= BIT4 ;    // P2.4 Port Map functions
    P2->SEL1 &= ~(BIT4);  // P2.4 default PM functions = TimerA CCRx
    P2->DIR |= BIT4;
    P2->DS |= BIT4;
}

void goToPosition0()
{
    pwmConfig.dutyCycle = 3000;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}
void goToPosition90()
{
    pwmConfig.dutyCycle = 4500;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}
void goToPosition180()
{
    pwmConfig.dutyCycle = 6000;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}



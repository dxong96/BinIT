/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

int a = 0;


/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
        //uint_fast16_t clockSource;
        TIMER_A_CLOCKSOURCE_SMCLK,
        //uint_fast16_t clockSourceDivider;
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        //uint_fast16_t timerPeriod;
        12800,
        //uint_fast16_t compareRegister;
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        //uint_fast16_t compareOutputMode;
        TIMER_A_OUTPUTMODE_RESET_SET,
        //uint_fast16_t dutyCycle;
        100
};

// PWM - PIN - 5.6 - TIMER_A2_BASE
// PWM - PIN - 2.4 - TIMER_A0_BASE

int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    /* Setting MCLK to REFO at 128Khz for LF mode setting SMCLK to 64Khz */
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

    // Configuring GPIO2.4 as peripheral output for PWM
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    //MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    pwmConfig.dutyCycle = 60;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
    // Configuring Timer_A to have a period of approximately 20ms
    //MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}

void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
    {
        pwmConfig.dutyCycle = 40;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
    else if(status & GPIO_PIN4){
        pwmConfig.dutyCycle = 140;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
}



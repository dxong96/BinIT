#include <stdbool.h>
#include <stdint.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>


//***** Definitions *****
#define CPU_FREQ                    (24000000)

//***** Function Prototypes *****
void startCrystalOscillator(void);
void setSystemClock(uint32_t CPU_Frequency);
void runServo(void);

//***** Global Data *****
// Timer_A PWM Configuration Parameter
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,       // 6MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,  // 93750??
        11720,                           // 250ms
        TIMER_A_CAPTURECOMPARE_REGISTER_0,
        TIMER_A_OUTPUTMODE_OUTBITVALUE /*TIMER_A_OUTPUTMODE_TOGGLE*/,
        11720
};


/***********************************************************
  Function:
*/

void main(void){

    WDT_A_holdTimer();
    runServo();

}

void runServo(void)
{
    MAP_Interrupt_disableMaster();                // Disabling master interrupts

    // Configure clocks
    startCrystalOscillator();
    setSystemClock(CPU_FREQ);

    // P1.1 for button interrupt
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);


    // P1.0 as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // Output MCLK and ACLK
//  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4,
//          GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // Timer A0
    // Set the Period for CC0
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    // Configuring GPIO 2.4 as peripheral output for PWM - TA0.1
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);

    // Set the Duty Cycle for CC1 - 10%
    pwmConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    pwmConfig.dutyCycle = 2344;
    pwmConfig.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);


    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;

    aclk = CS_getACLK();
    mclk = CS_getMCLK();
    smclk = CS_getSMCLK();
    hsmclk = CS_getHSMCLK();
    bclk = CS_getBCLK();

    /* Sleeping when not in use */
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
}

/***********************************************************
  Function:

   The following function is responsible for starting XT1 in the
   MSP432 that is used to source the internal FLL that drives the
   MCLK and SMCLK.
*/
void startCrystalOscillator(void)
{
    /* Configuring pins for peripheral/crystal HFXT*/
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring pins for peripheral/crystal LFXT*/
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
}

/***********************************************************
  Function:

   The following function is responsible for setting up the system
   clock at a specified frequency.
*/
void setSystemClock(uint32_t CPU_Frequency)
{
    /* Setting the external clock frequency. This API is optional, but will
     * come in handy if the user ever wants to use the getMCLK/getACLK/etc
     * functions
     */
    MAP_CS_setExternalClockSourceFrequency(32768, CPU_Frequency);

    /* Before we start we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_AM_LDO_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Starting HFXT and LFXT in non-bypass mode without a timeout. */
    MAP_CS_startHFXT(false);
    MAP_CS_startLFXT(false);

    /* Initializing the clock source as follows:
     *      MCLK = HFXT = 48MHz
     *      ACLK = LFXT = 32KHz
     *      HSMCLK = HFXT/4 = 12MHz
     *      SMCLK = HFXT/8 = 6MHz
     *      BCLK  = REFO = 32kHz
     */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_8);
    MAP_CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
}

/***********************************************************
  Function: port1IntHandler
 */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
    {
        // Timer A1
        /*if(pwmConfig.dutyCycle >= 10548)
        {
            pwmConfig.dutyCycle = 3164;
        }
        else
        {
            pwmConfig.dutyCycle += 5274;
        }*/

        pwmConfig.dutyCycle += 5274;

        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }

    if (status & GPIO_PIN4){

        /*if(pwmConfig.dutyCycle <= 0){
            pwmConfig.dutyCycle = 3164;
        }
        else
        {
            pwmConfig.dutyCycle -= 5274;
        }*/
        pwmConfig.dutyCycle -= 5274;

        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
}

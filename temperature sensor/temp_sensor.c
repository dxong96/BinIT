#include "temp_sensor.h"
#include "msp.h"

void readTempSensor(float * result) {
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 |  // SMCLK
                            TIMER_A_CTL_MC_2 |      // CONTINUOUS mode
                            TIMER_A_CTL_CLR;       // Clear TAR

    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1;
    P4->DIR &= ~BIT1;
    P4->REN |= BIT1;
    P4->OUT |= BIT1;
    _delay_cycles(3000);

    P4->REN &= ~BIT1;
    P4->DIR |= BIT1;
    P4->OUT &= ~BIT1;
    __delay_cycles(3300);

    P4->DIR &= ~BIT1;
    P4->REN |= BIT1;
    P4->OUT |= BIT1;

    __delay_cycles(165);

    while ((P4->IN & BIT1) != 0); // wait for 0

//    printf("got 0\n Waiting for 1\n");

    while ((P4->IN & BIT1) == 0); // wait for 1

//    printf("got 1\n");

    int cycles[80];
    int i, start;

    while ((P4->IN & BIT1) != 0); // wait for 0

    for (i = 0; i < 80; i += 2) {
        TIMER_A0->CTL |= TIMER_A_CTL_CLR;
        while ((P4->IN & BIT1) == 0); // waiting for 1
        cycles[i] = TIMER_A0->R;

        TIMER_A0->CTL |= TIMER_A_CTL_CLR;
        while ((P4->IN & BIT1) != 0); // waiting for 0
        cycles[i + 1] = TIMER_A0->R;
    }

    unsigned char data[5];
    for (i = 0; i < 40; ++i) {
        uint32_t lowCycles = cycles[2 * i];
        uint32_t highCycles = cycles[2 * i + 1];

        data[i / 8] <<= 1;
        // Now compare the low and high cycle times to see if the bit is a 0 or 1.
        if (highCycles > lowCycles) {
          // High cycles are greater than 50us low cycle count, must be a 1.
          data[i / 8] |= 1;
        }
        // Else high cycles are less than (or equal to, a weird case) the 50us low
        // cycle count so this must be a zero.  Nothing needs to be changed in the
        // stored data.
      }

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        result[0] = parseCelcius(data);
        result[1] = parseHumidity(data);
        return result;
    }
    return 0;
}

float parseCelcius(unsigned char * data) {
    float f = ((short)(data[2] & 0x7F)) << 8 | data[3];
    f *= 0.1;
    if (data[2] & 0x80) {
        f *= -1;
    }
    return f;
}

float parseHumidity(unsigned char * data) {
    float f =((short)data[0]) << 8 | data[1];
    f *= 0.1;
    return f;
}

#include "temp_sensor.h"
#include "msp.h"

void readTempSensor(float * result) {
    TIMER_A1->CTL = TIMER_A_CTL_TASSEL_2 |  // SMCLK
                            TIMER_A_CTL_MC_2 |      // CONTINUOUS mode
                            TIMER_A_CTL_CLR;       // Clear TAR

    P1->SEL0 &= ~BIT7;
    P1->SEL1 &= ~BIT7;
    P1->DIR &= ~BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;
    __delay_cycles(3000);

    P1->REN &= ~BIT7;
    P1->DIR |= BIT7;
    P1->OUT &= ~BIT7;
    __delay_cycles(3300);

    P1->DIR &= ~BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;

    __delay_cycles(165);

    while ((P1->IN & BIT7) != 0); // wait for 0


    while ((P1->IN & BIT7) == 0); // wait for 1

    int cycles[80];
    int i;

    while ((P1->IN & BIT7) != 0); // wait for 0

    for (i = 0; i < 80; i += 2) {
        TIMER_A1->CTL |= TIMER_A_CTL_CLR;
        while ((P1->IN & BIT7) == 0); // waiting for 1
        cycles[i] = TIMER_A1->R;

        TIMER_A1->CTL |= TIMER_A_CTL_CLR;
        while ((P1->IN & BIT7) != 0); // waiting for 0
        cycles[i + 1] = TIMER_A1->R;
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
    // checksum to check if the data is valid
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        result[0] = parseCelcius(data);
        result[1] = parseHumidity(data);
    }
}

float parseCelcius(unsigned char * data) {
    float f = ((short)(data[2] & 0x7F)) << 8 | data[3];
    f *= 0.1;
    // check if data is negative
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

#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

void Pattern0(int phase) {
    (void)phase;
    P5OUT = (P5OUT & ~0x0F) | 0b1010; //sets upper 4 bits (P5.3 - P5.0) to 1010
    P6OUT = (P6OUT & ~0x0F) | 0b1010; //sets lower 4 bits (P6.3 - P6.0) to 1010
    return;
}

void Pattern1(int phase) {
    if (phase == 0) { //checks if the phase is even or odd, below is even phase
        P5OUT = (P5OUT & ~0x0F) | 0b1010; //sets upper 4 bits (P5.3 - P5.0) to 1010
        P6OUT = (P6OUT & ~0x0F) | 0b1010; //sets lower 4 bits (P6.3 - P6.0) to 1010
    } else { //odd phase
        P5OUT = (P5OUT & ~0x0F) | 0b0101; //sets upper 4 bits (P5.3 - P5.0) to 0101
        P6OUT = (P6OUT & ~0x0F) | 0b0101; //sets lower 4 bits (P6.3 - P6.0) to 0101
    }
    return;
}
void Pattern2(int phase) {
    P5OUT = (P5OUT & ~0x0F) | ((phase >> 4) & 0x0F); //extracts upper 4 bits and assigns them to P5
    P6OUT = (P6OUT & ~0x0F) | (phase & 0x0F);        //extracts lower 4 bits and assigns them to P6
    return;
}
void Pattern3(int phase) {
    const uint8_t pattern[6] = {
        0b00011000,  //pattern 3 step 0
        0b00100100,  //pattern 3 step 1
        0b01000010,  //pattern 3 step 2
        0b10000001,  //pattern 3 step 3
        0b01000010,  //pattern 3 step 4
        0b00100100   //pattern 3 step 5
    };

    P5OUT = (P5OUT & ~0x0F) | (pattern[phase] >> 4);    //upper 4 bits for given phase
    P6OUT = (P6OUT & ~0x0F) | (pattern[phase] & 0x0F);  //lower 4 bits for given phase
    return;
}

void Pattern4(int phase) {
    uint8_t count = 255 - phase; //adjusts the step decrement if needed

    P5OUT = (P5OUT & ~0x0F) | ((count >> 4) & 0x0F); //upper 4 bits for given phase
    P6OUT = (P6OUT & ~0x0F) | (count & 0x0F);        //lower 4 bits for given phase
    return;
}

void Pattern5(int phase) {
    const uint8_t pattern[8] = {
        0b00000001,  //pattern 5 step 0
        0b00000010,  //pattern 5 step 1
        0b00000100,  //pattern 5 step 2
        0b00001000,  //pattern 5 step 3
        0b00010000,  //pattern 5 step 4
        0b00100000,  //pattern 5 step 5
        0b01000000,  //pattern 5 step 6
        0b10000000   //pattern 5 step 7
    };

    P5OUT = (P5OUT & ~0x0F) | (pattern[phase] >> 4);    //upper 4 bits for given phase
    P6OUT = (P6OUT & ~0x0F) | (pattern[phase] & 0x0F);  //lower 4 bits for given phase
    return;
}

void Pattern6(int phase) {
    const uint8_t pattern[8] = {
        0b01111111,  //pattern 6 step 0
        0b10111111,  //pattern 6 step 1
        0b11011111,  //pattern 6 step 2
        0b11101111,  //pattern 6 step 3
        0b11110111,  //pattern 6 step 4
        0b11111011,  //pattern 6 step 5
        0b11111101,  //pattern 6 step 6
        0b11111110   //pattern 6 step 7
    };

    P5OUT = (P5OUT & ~0x0F) | (pattern[phase] >> 4);    //upper 4 bits for given phase
    P6OUT = (P6OUT & ~0x0F) | (pattern[phase] & 0x0F);  //lower 4 bits for given phase
    return;
}

void Pattern7(int phase) {
    const uint8_t pattern[8] = {
        0b00000001,  //pattern 7 step 0
        0b00000011,  //pattern 7 step 1
        0b00000111,  //pattern 7 step 2
        0b00001111,  //pattern 7 step 3
        0b00011111,  //pattern 7 step 4
        0b00111111,  //pattern 7 step 5
        0b01111111,  //pattern 7 step 6
        0b11111111   //pattern 7 step 7
    };

    P5OUT = (P5OUT & ~0x0F) | (pattern[phase] >> 4);    //upper 4 bits for given phase
    P6OUT = (P6OUT & ~0x0F) | (pattern[phase] & 0x0F);  //lower 4 bits for given phase
    return;
}

void set_LCD_Timer(void) {
    TB2CTL = TBSSEL__SMCLK | ID_3 | MC__UP | TBCLR;
    TB2EX0 |= TBIDEX_4;
    TB2CCR0 = 25000; 
    TB2CCTL0 |= CCIE; 
    return;
}
void pause_LCD_Timer(void) {
    TB2CCTL0 &= ~CCIE; 
    TB2CCR0 = 25000; 
    return;
}

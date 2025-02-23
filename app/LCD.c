#include <msp430.h>
#include <stdbool.h>
#include <string.h>

void Pattern1(int lights) {
    if (lights == 0) {
        P5OUT &= ~0b00001111;
        P5OUT |= 0b00001111;
        P6OUT &= ~0b00001111;
        P6OUT |= 0b00001111;
    }
    else if (lights == 0) {
        P5OUT &= ~0b00001111;
        P5OUT |= 0b00001111;
        P6OUT &= ~0b00001111;
        P6OUT |= 0b00001111;
    }
    else {

    }
    return;
}
void Pattern2(int lights) {
    return;
}
void Pattern3(int lights) {
    return;
}

void set_LCD_Timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; 
    TB0CCR0 = 78; 
    TB0CCTL0 |= CCIE; 
}

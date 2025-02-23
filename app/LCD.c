#include <msp430.h>
#include <stdbool.h>
#include <string.h>

void Pattern1(int phase) {
    if (phase == 0) { // Check if phase is even
        P5OUT = (P5OUT & ~0x0F) | 0b1010; // Set upper 4 bits (P5.3 - P5.0) to 1010
        P6OUT = (P6OUT & ~0x0F) | 0b1010; // Set lower 4 bits (P6.3 - P6.0) to 1010
    } else { // If phase is odd
        P5OUT = (P5OUT & ~0x0F) | 0b0101; // Set upper 4 bits to 0101
        P6OUT = (P6OUT & ~0x0F) | 0b0101; // Set lower 4 bits to 0101
    }
    return;
}
void Pattern2(int phase) {
    P5OUT = (P5OUT & ~0x0F) | ((phase >> 4) & 0x0F); // Extract upper 4 bits and assign to P5
    P6OUT = (P6OUT & ~0x0F) | (phase & 0x0F); // Extract lower 4 bits and assign to P6
    return;
}
void Pattern3(int phase) {
        switch (phase) { // Cycle through six steps
        case 0:
            P5OUT = (P5OUT & ~0x0F) | 0b0001; 
            P6OUT = (P6OUT & ~0x0F) | 0b1000; 
            break;
        case 1:
            P5OUT = (P5OUT & ~0x0F) | 0b0010; 
            P6OUT = (P6OUT & ~0x0F) | 0b0100; 
            break;
        case 2:
            P5OUT = (P5OUT & ~0x0F) | 0b0100; 
            P6OUT = (P6OUT & ~0x0F) | 0b0010; 
            break;
        case 3:
            P5OUT = (P5OUT & ~0x0F) | 0b1000; 
            P6OUT = (P6OUT & ~0x0F) | 0b0001; 
            break;
        case 4:
            P5OUT = (P5OUT & ~0x0F) | 0b0100; 
            P6OUT = (P6OUT & ~0x0F) | 0b0010; 
            break;
        case 5:
            P5OUT = (P5OUT & ~0x0F) | 0b0010; 
            P6OUT = (P6OUT & ~0x0F) | 0b0100; 
            break;
    }
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

#include <msp430.h>
#include <stdbool.h>
#include "LCD.h"

// Pattern 1: Toggle (10101010 -> 01010101) with P5.3 as MSB and P6.0 as LSB
void Pattern1(int phase) {
    if (phase % 2 == 0) { // Check if phase is even
        P5OUT = (P5OUT & ~0x0F) | 0b1010; // Set upper 4 bits (P5.3 - P5.0) to 1010
        P6OUT = (P6OUT & ~0x0F) | 0b1010; // Set lower 4 bits (P6.3 - P6.0) to 1010
    } else { // If phase is odd
        P5OUT = (P5OUT & ~0x0F) | 0b0101; // Set upper 4 bits to 0101
        P6OUT = (P6OUT & ~0x0F) | 0b0101; // Set lower 4 bits to 0101
    }
}

// Pattern 2: Up Counter (Counts from 0 to 255 in binary)
void Pattern2(int phase) {
    P5OUT = (P5OUT & ~0x0F) | ((phase >> 4) & 0x0F); // Extract upper 4 bits and assign to P5
    P6OUT = (P6OUT & ~0x0F) | (phase & 0x0F); // Extract lower 4 bits and assign to P6
}

// Pattern 3: In and Out movement
void Pattern3(int phase) {
    switch (phase % 6) { // Cycle through six steps
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
}

// Timer configuration for pattern transitions
void set_LCD_Timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // Use SMCLK, set to up mode, clear timer
    TB0CCR0 = 78; // 
    TB0CCTL0 |= CCIE; // Enable timer interrupt
}

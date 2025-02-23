#include <msp430.h> 

void timerB1_init(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;                   // Stops watchdog timer

    P1DIR |= BIT0;                              // Sets P1.0 as an output
    P1OUT &= ~BIT0;                             // Initializes LED to OFF

    PM5CTL0 &= ~LOCKLPM5;                       // Unlocks GPIO

    timerB1_init();                             // Initializes B1 timer

    __bis_SR_register(LPM3_bits | GIE);         // LPM3 for ACLK usage, GIE enables interrupts, enter low power mode

    while (1) {
                                                // Main loop empty, ISR handles LED toggling
    }
}

void timerB1_init(void) {
    TB1CCTL0 = CCIE;                            // CCIE enables Timer B1 interrupt
    TB1CCR0 = 32768;                            // Sets Timer B1 to 1 second (32.768 kHz)
    TB1CTL = TBSSEL_1 | ID_0 | MC_1 | TBCLR;    // ACLK, No divider, Up mode, Clear timer
}

#pragma vector = TIMER1_B0_VECTOR               // Timer B1 ISR
__interrupt void TIMERB1_ISR(void) {
    P1OUT ^= BIT0;                              // Toggles P1.0 LED
}

#include <msp430.h>

void timerB0_init(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;                   //stops watchdog timer


    P1DIR |= BIT0;                              //sets P1.0 as an output
    P1OUT &= ~BIT0;                             //initializes LED to OFF

    PM5CTL0 &= ~LOCKLPM5;                       //unlocks GPIO

    timerB0_init();                             //initializes B timer

    __bis_SR_register(LPM3_bits | GIE);         // LPM3 for ACLK usage, GIE enables interrupts, enter low power mode

    while (1) {
                                                //main loop empty, ISR handles LED changeing
    }
}

void timerB0_init(void) {
    TB0CCTL0 = CCIE;                            //CCIE enables Timer B0 interrupt
    TB0CCR0 = 32768;                            //sets Timer B0 to 1 second (32.768 kHz)
    TB0CTL = TBSSEL_1 | ID_0 | MC_1 | TBCLR;    //ACLK, No divider, Up mode, Clear timer
}

#pragma vector = TIMER0_B0_VECTOR               //time B0 ISR
__interrupt void TIMERB0_ISR(void) {
    P1OUT ^= BIT0;                              //toggles P1.0 LED
}

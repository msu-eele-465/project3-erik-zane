#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "LCD.h"
#include "keypad.h"
#include "RGB.h"
#include "shared.h"

#define RED_LED   BIT1 
#define GREEN_LED BIT2 
#define BLUE_LED  BIT3 

volatile unsigned int red_counter = 0;
volatile unsigned int green_counter = 0;
volatile unsigned int blue_counter = 0;

volatile unsigned int limit_reached = 0;
volatile unsigned int pwms = 0;
volatile unsigned int next_pattern = 0;

volatile system_states state = LOCKED;

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P3DIR |= 0b00001111;   // set keypad columns to outputs pulled high
    P3OUT |= 0b00001111;

    P5DIR |= 0b00001111;   // 4 MSBs of LCD display
    P5OUT &= ~0b00001111;
    P6DIR |= 0b00001111;   // 4 LSBs of LCD display
    P6OUT &= ~0b00001111;

    //heartbeat LED
    P1DIR |= BIT0;                              // Sets P1.0 as an output
    P1OUT &= ~BIT0;                             // Initializes LED to OFF

    P6DIR |= BIT6;                              // LED for debugging       
    P6OUT &= ~BIT6;

    P3DIR &= ~0b11110000; // set all keypad rows to inputs pulled low
    P3REN |= 0b11111111; // permanently set all of port 3 to have resistors
    P3OUT &= ~0b11110000; // pull down resistors

    P1DIR |= RED_LED | GREEN_LED | BLUE_LED;
    P1OUT |= RED_LED | GREEN_LED | BLUE_LED;  // Start with all ON

    //heartbeat interrupt
    TB1CCTL0 = CCIE;                            //CCIE enables Timer B0 interrupt
    TB1CCR0 = 32768;                            //sets Timer B0 to 1 second (32.768 kHz)
    TB1CTL = TBSSEL_1 | ID_0 | MC__UP | TBCLR;    //ACLK, No divider, Up mode, Clear timer

    // set timer for password
    TB3CTL = TBSSEL__ACLK | ID_0 | MC__UP | TBCLR;
    TB3EX0 |= TBIDEX_4; // divide by 5
    TB3CCR0 = 1; // 1 for now to ensure that timer virtually starts at 0, 35000  is slightly more than 5 seconds
    TB3CCTL0 &= ~CCIE; // do not trigger interrupts yet

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configure port settings

    PM5CTL0 &= ~LOCKLPM5;

    set_timer(); 

    __enable_interrupt(); 
    int unlock = 0;

    while(true)
    {
        unlock = 0;
        state = LOCKED;
        update_color(state);
        while (unlock == 0) {
            unlock = waitForUnlock(); // stays here until complete passkey has been entered 
        }
        // turn status LED Blue here
        state = UNLOCKED;
        update_color(state);
        char lastInput = 'X';
        while (lastInput != '0' && lastInput != '1' && lastInput != '2' && lastInput != '3' && lastInput != '4' && lastInput != '5' && lastInput != '6' && lastInput != '7' && lastInput != 'D') {
            lastInput = readInput(); // stays here until user chooses a pattern, or chooses to lock the system
        }
        char chosenPattern = lastInput;
        int rows;
        int phase0 = 0;
        int phase1 = 0;
        int phase2 = 0;
        int phase3 = 0;
        int phase4 = 0;
        int phase5 = 0;
        int phase6 = 0;
        int phase7 = 0;
        set_LCD_Timer(); // enable LCD-pattern-trigger timer interrupt here
        while (lastInput != 'D') {
            if (chosenPattern == '1') {
                Pattern1(phase1); // set default (initial) light pattern for pattern 1
                while (chosenPattern == '1') { 
                    if (phase1 < 1) {
                        phase1 ++;
                    }
                    else {
                        phase1 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '2' || lastInput == '3' || lastInput == '4' || lastInput == '5' || lastInput == '6' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '1') {
                                phase1 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern1(phase1);
                    next_pattern = 0;
                }
            }     
                
            else if (chosenPattern == '2') {
                Pattern2(phase2); // set default (initial) light pattern for pattern 2
                while (chosenPattern == '2') { 
                    if (phase2 < 255) {
                        phase2 ++;
                    }
                    else {
                        phase2 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '3' || lastInput == '4' || lastInput == '5' || lastInput == '6' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '2') {
                                phase2 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern2(phase2);
                    next_pattern = 0;
                }
            }
            else if (chosenPattern == '3') {
                Pattern3(phase3); // set default (initial) light pattern for pattern 3
                while (chosenPattern == '3') { 
                    if (phase3 < 5) {
                        phase3 ++;
                    }
                    else {
                        phase3 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '2' || lastInput == '4' || lastInput == '5' || lastInput == '6' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '3') {
                                phase3 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern3(phase3);
                    next_pattern = 0;
                }
            }

            else if (chosenPattern == '0') {
                Pattern0(phase0); // set default (initial) light pattern for pattern 0
                while (chosenPattern == '0') { 
                    if (next_pattern) {
                        next_pattern = 0;
                        Pattern0(phase0); // static so phase isn't needed
                    }
                    rows = P3IN; // constantly listen for an input
                    rows &= 0b11110000; // clear any values on lower 4 bits
                    if (rows != 0b00000000) {
                        lastInput = readInput();
                        if (lastInput == 'D') {
                            chosenPattern = 'D'; // lock device
                        } 
                        else if (lastInput == '1' || lastInput == '2' || lastInput == '3' || lastInput == '4' || lastInput == '5' || lastInput == '6' || lastInput == '7') {
                            chosenPattern = lastInput;
                        }
                        else if (lastInput == '0') {
                                phase0 = 0;
                            }
                        else {

                        }
                    }
                }
            }

            else if (chosenPattern == '4') {
                Pattern4(phase4); // set default (initial) light pattern for pattern 4
                while (chosenPattern == '4') { 
                    if (phase4 < 255) {
                        phase4++;
                    } else {
                        phase4 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            } 
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                TB2CCR0 -= 6250;
                            } 
                            else if (lastInput == 'B') {
                                TB2CCR0 += 6250;
                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '2' || lastInput == '3' || lastInput == '5' || lastInput == '6' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '4') {
                                phase4 = 0;
                            }
                            else {

                            }
                        }
                    }
                    Pattern4(phase4);
                    next_pattern = 0;
                }
            }

            else if (chosenPattern == '5') {
                Pattern5(phase5); // set default (initial) light pattern for pattern 5
                while (chosenPattern == '5') { 
                    if (phase5 < 7) {
                        phase5 ++;
                    }
                    else {
                        phase5 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '2' || lastInput == '3' || lastInput == '4' || lastInput == '6' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '5') {
                                phase5 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern5(phase5);
                    next_pattern = 0;
                }
            }

            else if (chosenPattern == '6') {
                Pattern6(phase6); // set default (initial) light pattern for pattern 6
                while (chosenPattern == '6') { 
                    if (phase6 < 7) {
                        phase6 ++;
                    }
                    else {
                        phase6 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '2' || lastInput == '3' || lastInput == '4' || lastInput == '5' || lastInput == '7') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '6') {
                                phase6 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern6(phase6);
                    next_pattern = 0;
                }
            }

            else if (chosenPattern == '7') {
                Pattern7(phase7); // set default (initial) light pattern for pattern 7
                while (chosenPattern == '7') { 
                    if (phase7 < 7) {
                        phase7 ++;
                    }
                    else {
                        phase7 = 0;
                    }
                    while (next_pattern == 0) {
                        rows = P3IN; // constantly listen for an input
                        rows &= 0b11110000; // clear any values on lower 4 bits
                        if (rows != 0b00000000) {
                            lastInput = readInput();
                            if (lastInput == 'D') {
                                chosenPattern = 'D'; // locks device
                            }
                            else if (lastInput == 'A' && TB2CCR0 > 7000) {
                                // if possible, subtract .25 seconds from interval
                                TB2CCR0 = TB2CCR0 - 6250;
                            }
                            else if (lastInput == 'B') {
                                // add .25 seconds to interval
                                TB2CCR0 = TB2CCR0 + 6250;

                            }
                            else if (lastInput == '0' || lastInput == '1' || lastInput == '2' || lastInput == '3' || lastInput == '4' || lastInput == '5' || lastInput == '6') {
                                chosenPattern = lastInput;
                            }
                            else if (lastInput == '7') {
                                phase7 = 0;
                            }
                            else {
                                // useless input, so do nothing
                            }
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern7(phase7);
                    next_pattern = 0;
                }
            }

            else {
                lastInput = readInput();
                chosenPattern = lastInput;
            }
        }
        P5OUT &= ~0b00001111;
        P6OUT &= ~0b00001111; // turn LCD off completely
        pause_LCD_Timer(); // disable LCD-pattern-trigger timer interrupt here (system returns to locked state)
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {

    pwms = (pwms + 1) % 256;

    // Red LED
    if (pwms == red_counter){
        P1OUT &= ~RED_LED;
    }
    if (pwms == 0) {
        P1OUT |= RED_LED;
    }

    // Green LED
    if (pwms == green_counter) {
        P1OUT &= ~GREEN_LED;
    }
    if (pwms == 0) {
        P1OUT |= GREEN_LED;
    }

    // Blue LED
    if (pwms == blue_counter) {
        P1OUT &= ~BLUE_LED;
    }
    if (pwms == 0) {
        P1OUT |= BLUE_LED;
    }
    TB0CCTL0 &= ~CCIFG;

}

#pragma vector = TIMER1_B0_VECTOR               //time B0 ISR
__interrupt void TIMERB1_ISR(void) {
    P1OUT ^= BIT0;                              //toggles P1.0 LED
    TB1CCTL0 &= ~CCIFG;
}
//todo: setup LCD timing (ISR)
#pragma vector = TIMER2_B0_VECTOR
__interrupt void TIMERB2_ISR(void) {
    next_pattern = 1;
    TB2CCTL0 &= ~CCIFG;
} 

#pragma vector = TIMER3_B0_VECTOR
__interrupt void TIMERB3_ISR(void) {
    limit_reached = 1;
    TB3CCTL0 &= ~CCIFG;
} 
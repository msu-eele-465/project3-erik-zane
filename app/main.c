#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "LCD.h"
#include "keypad.h"
#include "RGB.h"
#include "shared.h"

//#define RED_LED   BIT2 
//#define GREEN_LED BIT3 
//#define BLUE_LED  BIT4 

volatile unsigned int red_counter = 0;
volatile unsigned int green_counter = 0;
volatile unsigned int blue_counter = 0;

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

    P1DIR |= BIT0;                              // Sets P1.0 as an output
    P1OUT &= ~BIT0;                             // Initializes LED to OFF

    P3DIR &= ~0b11110000; // set all keypad rows to inputs pulled low
    P3REN |= 0b11111111; // permanently set all of port 3 to have resistors
    P3OUT &= ~0b11110000; // pull down resistors


    P1DIR |= 0b00001110;
    P1OUT |= 0b00001110;
    //P1DIR |= RED_LED | GREEN_LED | BLUE_LED;
    //P1OUT |= RED_LED | GREEN_LED | BLUE_LED;  // Start with all ON

    TB1CCTL0 = CCIE;                            //CCIE enables Timer B0 interrupt
    TB1CCR0 = 32768;                            //sets Timer B0 to 1 second (32.768 kHz)
    TB1CTL = TBSSEL_1 | ID_0 | MC__UP | TBCLR;    //ACLK, No divider, Up mode, Clear timer
    
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
        char lastInput = '0';
        while (lastInput != '1' && lastInput != '2' && lastInput != '3' && lastInput != 'D') {
            lastInput = readInput(); // stays here until user chooses a pattern, or chooses to lock the system
        }
        char chosenPattern = lastInput;
        int rows;
        int phase;
        // likely enable LCD-pattern-trigger timer interrupt here
        while (lastInput != 'D') {
            if (chosenPattern == '1') {
                phase = 0;
                Pattern1(phase); // set default (initial) light pattern for pattern 1
                while (chosenPattern == '1') { 
                    if (phase < 1) {
                        phase ++;
                    }
                    else {
                        phase = 0;
                    }
                    rows = P3IN; // constantly listen for an input
                    rows &= 0b11110000; // clear any values on lower 4 bits
                    if (rows != 0b00000000) {
                        lastInput = readInput();
                        if (lastInput == 'D') {
                            chosenPattern = 'D'; // locks device
                        }
                        else if (lastInput == 'A') {
                            // if possible, subtract .25 seconds from interval
                        }
                        else if (lastInput == 'B') {
                            // add .25 seconds to interval
                        }
                        else if (lastInput == '2' || lastInput == '3') {
                            chosenPattern = lastInput;
                        }
                        else {
                            // useless input, so do nothing
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern1(phase);
                }
            }     
                
            else if (chosenPattern == '2') {
                phase = 0;
                Pattern2(phase); // set default (initial) light pattern for pattern 2
                while (chosenPattern == '2') { 
                    if (phase < 5) {
                        phase ++;
                    }
                    else {
                        phase = 0;
                    }
                    rows = P3IN; // constantly listen for an input
                    rows &= 0b11110000; // clear any values on lower 4 bits
                    if (rows != 0b00000000) {
                        lastInput = readInput();
                        if (lastInput == 'D') {
                            chosenPattern = 'D'; // locks device
                        }
                        else if (lastInput == 'A') {
                            // if possible, subtract .25 seconds from interval
                        }
                        else if (lastInput == 'B') {
                            // add .25 seconds to interval
                        }
                        else if (lastInput == '1' || lastInput == '3') {
                            chosenPattern = lastInput;
                        }
                        else {
                            // useless input, so do nothing
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern2(phase);
                }

            }
            else if (chosenPattern == '3') {
                phase = 0;
                Pattern3(phase); // set default (initial) light pattern for pattern 3
                while (chosenPattern == 3) { 
                    if (phase < 7) {
                        phase ++;
                    }
                    else {
                        phase = 0;
                    }
                    rows = P3IN; // constantly listen for an input
                    rows &= 0b11110000; // clear any values on lower 4 bits
                    if (rows != 0b00000000) {
                        lastInput = readInput();
                        if (lastInput == 'D') {
                            chosenPattern = 'D'; // locks device
                        }
                        else if (lastInput == 'A') {
                            // if possible, subtract .25 seconds from interval
                        }
                        else if (lastInput == 'B') {
                            // add .25 seconds to interval
                        }
                        else if (lastInput == '1' || lastInput == '2') {
                            chosenPattern = lastInput;
                        }
                        else {
                            // useless input, so do nothing
                        }
                    }
                    // wait for interval to end (likely use timer ISR to set variable), however, trial-error while-loops also likely work
                    Pattern3(phase);
                }
            }
        }
        P5OUT &= ~0b00001111;
        P6OUT &= ~0b00001111; // turn LCD off completely
        // likely disable LCD-pattern-trigger timer interrupt here (system returns to locked state)
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {
    static unsigned int pwms = 0;

    pwms = (pwms + 1) % 255;

    // Red LED
    if (pwms == red_counter){
        P1OUT &= ~0b00000010;
    }
    if (pwms == 0) {
        P1OUT |= 0b00000010;
    }

    // Green LED
    if (pwms == green_counter) {
        P1OUT &= ~0b00000100;
    }
    if (pwms == 0) {
        P1OUT |= 0b00000100;
    }

    // Blue LED
    if (pwms == blue_counter) {
        P1OUT &= ~0b00001000;
    }
    if (pwms == 0) {
        P1OUT |= 0b00001000;
    }
    TB0CCTL0 &= ~CCIFG;

}

#pragma vector = TIMER1_B0_VECTOR               //time B0 ISR
__interrupt void TIMERB1_ISR(void) {
    P1OUT ^= BIT0;                              //toggles P1.0 LED
    TB1CCTL0 &= ~CCIFG;
}
//todo: setup LCD timing (ISR)
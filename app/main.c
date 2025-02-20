#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "LCD.c"
#include "keyPad.c"
#include "RGB.c"

#define RED_LED   BIT2 
#define GREEN_LED BIT3 
#define BLUE_LED  BIT4 

volatile unsigned int red_counter = 0;
volatile unsigned int green_counter = 0;
volatile unsigned int blue_counter = 0;

typedef enum { LOCKED, UNLOCKING, UNLOCKED } system_states;
volatile system_states state = LOCKED;

void setup_timer(void);
void update_color(system_states new_state);

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P3DIR |= 0b00001111;   // set keypad columns to outputs pulled high
    P3OUT |= 0b00001111;

    P3DIR &= ~0b11110000; // set all keypad rows to inputs pulled low
    P3REN |= 0b11111111; // permanently set all of port 3 to have resistors
    P3OUT &= ~0b11110000; // pull down resistors

    P1DIR |= RED_LED | GREEN_LED | BLUE_LED;
    P1OUT |= RED_LED | GREEN_LED | BLUE_LED;  // Start with all OFF
    
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configure port settings

    PM5CTL0 &= ~LOCKLPM5;

    set_timer(); 

    __enable_interrupt(); 

    while(true)
    {
        int unlock = 0;
        update_color(LOCKED);
        while (unlock == 0) {
            unlock = waitForUnlock(); // stays here until complete passkey has been entered 
        }
        // turn status LED Blue here
        update_color(UNLOCKED);
        char lastInput = '0';
        while (lastInput != '1' && lastInput != '2' && lastInput != '3' && lastInput != 'D') {
            lastInput = readInput(); // stays here until user chooses a pattern, or chooses to lock the system
        }
        char chosenPattern = lastInput;
        int rows;
        // likely enable LCD-pattern-trigger timer interrupt here
        while (lastInput != 'D') {
            if (chosenPattern == '1') {
                Pattern1(1); // set default (initial) light pattern for pattern 1
                while (chosenPattern == '1') { 
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
                    Pattern1(0);
                }
            }     
                
            else if (chosenPattern == '2') {
                Pattern2(1); // set default (initial) light pattern for pattern 2
                while (chosenPattern == '2') { 
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
                    Pattern2(0);
                }

            }
            else if (chosenPattern == '3') {
                Pattern3(1); // set default (initial) light pattern for pattern 3
                while (chosenPattern == 3) { 
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
                    Pattern3(0);
                }
            }
        }
        // turn LCD off completely
        // likely disable LCD-pattern-trigger timer interrupt here (system returns to locked state)
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {
    static unsigned int pwms = 0;

    pwms = (pwms + 1) % 256;

    // Red LED
    if (pwms == red_counter)
        P1OUT &= ~RED_LED;
    if (pwms == 0)
        P1OUT |= RED_LED;

    // Green LED
    if (pwms == green_counter)
        P1OUT &= ~GREEN_LED;
    if (pwms == 0)
        P1OUT |= GREEN_LED;

    // Blue LED
    if (pwms == blue_counter)
        P1OUT &= ~BLUE_LED;
    if (pwms == 0)
        P1OUT |= BLUE_LED;
}
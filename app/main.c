#include <msp430.h>
#include <stdbool.h>

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P3DIR |= 0b00001111;   // set keypad columns to outputs pulled high
    P3OUT |= 0b00001111;

    P3DIR &= ~0b11110000; // set all keypad rows to inputs pulled low
    P3REN |= 0b11111111; // permanently set all of port 3 to have resistors
    P3OUT &= ~0b11110000; // pull down resistors

    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    while(true)
    {
        int unlock = 0;
        while (unlock == 0) {
            unlock = waitForUnlock(); // stays here until complete passkey has been entered 
        }
        // turn status LED Blue here
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
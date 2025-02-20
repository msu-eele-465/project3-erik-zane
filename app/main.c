#include <msp430.h>
#include <stdbool.h>

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

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  

    P1DIR |= RED_LED | GREEN_LED | BLUE_LED;
    P1OUT &= ~(RED_LED | GREEN_LED | BLUE_LED);  // Start with all OFF

    PM5CTL0 &= ~LOCKLPM5;

    set_timer(); 

    __enable_interrupt(); 

    while (1) {
        __delay_cycles(5000000);
        update_color(UNLOCKING);
        __delay_cycles(5000000);
        update_color(UNLOCKED);
        __delay_cycles(5000000);
        update_color(LOCKED);
    }
}


void set_timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; 
    TB0CCR0 = 3750; 
    TB0CCTL0 |= CCIE; 
}


void update_color(system_states new_state) {
    state = new_state;

    switch (state) {
        case LOCKED:  // Red (#c43e1d)
            red_counter = 200;
            green_counter = 62;
            blue_counter = 29;
            break;

        case UNLOCKING:  // Yellow (#c4921d)
            red_counter = 200;
            green_counter = 146;
            blue_counter = 29;
            break;

        case UNLOCKED:  // Blue (#1da2c4)
            red_counter = 29;
            green_counter = 162;
            blue_counter = 196;
            break;
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {
    static unsigned int pwms = 0;

    pwms = (pwms + 1) % 256;

    // Red LED
    if (pwms < red_counter)
        P1OUT |= RED_LED;
    else
        P1OUT &= ~RED_LED;

    // Green LED
    if (pwms < green_counter)
        P1OUT |= GREEN_LED;
    else
        P1OUT &= ~GREEN_LED;

    // Blue LED
    if (pwms < blue_counter)
        P1OUT |= BLUE_LED;
    else
        P1OUT &= ~BLUE_LED;
}

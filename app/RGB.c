#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "shared.h"
#include "RGB.h"

void set_timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; 
    TB0CCR0 = 78; 
    TB0CCTL0 |= CCIE; 
}


void update_color(system_states new_state) { // system_states new_state) {
   system_states state = new_state;

    switch (state) {
        case LOCKED:  // Red (#c43e1d)
            red_counter = 255; // baseline 196
            green_counter = 80; // baseline 62
            blue_counter = 37; // baseline 29
            break;

        case UNLOCKING:  // Yellow (#c4921d)
            red_counter = 255; // baseline 196
            green_counter = 189; // baseline 146
            blue_counter = 37; // baseline 29
            break;

        case UNLOCKED:  // Blue (#1da2c4)
            red_counter = 37; // baseline 29
            green_counter = 210; // baseline 162
            blue_counter = 255; // baseline 196
            break;
    }
}

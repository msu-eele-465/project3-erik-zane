

void set_timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; 
    TB0CCR0 = 78; 
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
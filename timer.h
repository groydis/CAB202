#ifndef TIMER_H_
#define TIMER_H_

#include <stdlib.h>

#define FREQ (8000000.0)
#define PRESCALE (1024.0)

#define THRESHOLD (1000)

double minutes = 0;
double time = 0;

char buffer[20];

volatile int overflow_counter = 0;

ISR(TIMER0_OVF_vect) {
    overflow_counter ++;
    if (round(overflow_counter) == 1800){
        overflow_counter = 0;
        minutes++;
    }
}

void timer ( void ) {
	time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE  / FREQ;
}

void display_time(uint8_t x, uint8_t y) {
	snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, time);
    draw_string(x, y, buffer, FG_COLOUR);
}

#endif
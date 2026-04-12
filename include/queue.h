#ifndef KEYPAD_H
#define KEYPAD_H

#include <hardware/timer.h>
#include "pico/stdlib.h"

void keypad_init_pins();
void keypad_init_timer();
void display_init_pins();
void display_init_timer();
void display_print(const uint16_t message[]);
uint16_t key_pop();
void key_push(uint16_t value);

// Basic queue structure for tracking events
// Tracks 32 events, with each event being {pressed, key}
typedef struct {
    uint16_t q[32];
    uint16_t head;
    uint16_t tail;
} KeyEvents;

extern KeyEvents kev; // Global key event queue 

#endif
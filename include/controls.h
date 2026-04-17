#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdbool.h>

typedef enum {
    INPUT_DIRECTION_NONE,
    INPUT_DIRECTION_UP,
    INPUT_DIRECTION_DOWN,
    INPUT_DIRECTION_LEFT,
    INPUT_DIRECTION_RIGHT
} InputDirection;

typedef struct {
    InputDirection joystick;
    bool start_pressed;
    bool select_pressed;
} InputState;

void controls_init(void);

void controls_update(void);

InputState controls_get(void);

#endif

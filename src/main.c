#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display.h"
#include "controls.h"
#include "queue.h"

// For Keypad Testing
uint16_t key_pop() {
    // Queue is empty
    while (kev.head == kev.tail) {
        // sleep_ms(10);   // Wait for an event to be pushed
        return 0;
    }
    uint16_t value = kev.q[kev.tail];
    kev.tail = (kev.tail + 1) % 32;
    return value;
}

void key_push(uint16_t value) {
    // Queue is full, drop new keys
    if ((kev.head + 1) % 32 == kev.tail) {
        return; 
    }
    kev.q[kev.head] = value;
    kev.head = (kev.head + 1) % 32;
}

// Controls Struct (for testing rn)
InputState current_input = {
    .joystick = INPUT_DIRECTION_NONE,
    .start_pressed = false,
    .select_pressed = false
};

// Pacman struct, contains initial position
PacmanState pacman = {
    .y = 23,  //23
    .x = 13,  //13
    .lasty = 23,
    .lastx = 13,
    .direction = FACING_RIGHT
};

GhostState redghost = {
    .y = 14,
    .x = 12,
    .lasty = 14,
    .lastx = 12,
    .color = COLOR_RED
};

GhostState pinkghost = {
    .y = 14,
    .x = 15,
    .lasty = 14,
    .lastx = 15,
    .color = COLOR_PINK
};

GhostState blueghost = {
    .y = 11,
    .x = 18,
    .lasty = 11,
    .lastx = 18,
    .color = COLOR_BLUE
};

GhostState orangeghost = {
    .y = 11,
    .x = 9,
    .lasty = 11,
    .lastx = 9,
    .color = COLOR_ORANGE
};

// Game State global variable
GameState game_state = STARTING_MENU;

int main(){
    
    stdio_init_all();
    
    // Initialize the display and draw the map
    display_init();
    tft_fill_screen(BLACK);
    sleep_ms(100);

    // Initialize keypad pins and timer for testing
    keypad_init_pins();
    keypad_init_timer();
    
    // Game loop
    for(;;){

        // FOR TESTING WITH KEYPAD v =======
        uint32_t keyevent = key_pop();
        if (keyevent & (1 << 8)) {
            char button = (char) (keyevent & 0xFF);
            if(button == 'C'){
                current_input.joystick = INPUT_DIRECTION_UP;
            }
            else if(button == '#'){
                current_input.joystick = INPUT_DIRECTION_RIGHT;
            }
            else if(button == '8'){
                current_input.joystick = INPUT_DIRECTION_DOWN;
            }
            else if(button == '6'){
                current_input.joystick = INPUT_DIRECTION_LEFT;
            }
            else if(button == '*'){
                current_input.start_pressed = true;
            }
        }
        // FOR TESTING WITH KEYPAD ^ =======


        if(game_state == STARTING_MENU)
        {       
            // Draw the start screen
            draw_start_screen();

            // Pop a key and check if start ('*') is pressed
            keyevent = key_pop();
            game_state = check_start_pressed(keyevent, &current_input, pacman, pinkghost, redghost, blueghost, orangeghost);
        }
        else if(game_state == GAME_OVER){
            tft_fill_screen(BLACK);

            draw_end_screen();
        }

        else if(game_state == GAMEPLAY){

        // Update Pacman pos
        update_pacman(current_input, &pacman);

        // TODO : Update Ghost Pos

        // Draw Pacman updated pos
        draw_pacman(current_input, pacman);

        // Draw Ghost Pos
        draw_ghost(redghost);
        draw_ghost(blueghost);
        draw_ghost(pinkghost);
        draw_ghost(orangeghost);

        // TODO : Check pacman collision w/ ghosts
        game_state = check_collision(pacman, redghost, pinkghost, blueghost, orangeghost);

        sleep_ms(100); // Must wait so pacman doesnt move like hes on crack
        }
    }
}

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
volatile PacmanState pacman = {
    .y = 23,  //23
    .x = 13,  //13
    .lasty = 23,
    .lastx = 13,
    .direction = FACING_RIGHT,
    .mode = NORMAL
};

GhostState redghost = {
    .y = HOUSE_START_LEFT_Y,
    .x = HOUSE_START_LEFT_X,
    .lasty = HOUSE_START_LEFT_Y,
    .lastx = HOUSE_START_LEFT_X,
    .color = COLOR_RED,
    .location = IN_HOUSE,
    .unlock_counter = 6
};

GhostState pinkghost = {
    .y = HOUSE_START_RIGHT_Y,
    .x = HOUSE_START_RIGHT_X,
    .lasty = HOUSE_START_RIGHT_Y,
    .lastx = HOUSE_START_RIGHT_X,
    .color = COLOR_PINK,
    .location = IN_HOUSE,
    .unlock_counter = 6
};

GhostState blueghost = {
    .y = OUT_START_RIGHT_Y,
    .x = OUT_START_RIGHT_X,
    .lasty = OUT_START_RIGHT_Y,
    .lastx = OUT_START_RIGHT_X,
    .color = COLOR_BLUE,
    .location = OUT_HOUSE,
    .unlock_counter = 6
};

GhostState orangeghost = {
    .y = OUT_START_LEFT_Y,
    .x = OUT_START_LEFT_X,
    .lasty = OUT_START_LEFT_Y,
    .lastx = OUT_START_LEFT_X,
    .color = COLOR_ORANGE,
    .location = OUT_HOUSE,
    .unlock_counter = 6
};

volatile ScoreBoard scoreboard = {
    .score = 0,
    .lives = 3,
    .num_pellets = 237,
    .num_powers = 4,
    .total_food = 241
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

    // Init joysitck and buttons
    controls_init();

    // Initialize the Chomper interrupt (mode for when you pick up a powerup)
    init_chomper_timer();

    // Initialize ghost cage unlock timer
    // init_ghostunlock_timer();
    
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

        controls_update();
        current_input = controls_get();


        if(game_state == STARTING_MENU)
        {       
            // Draw the start screen
            draw_start_screen();

            // Pop a key and check if start ('*') is pressed
            keyevent = key_pop();
            controls_update();
            current_input = controls_get();
            
            game_state = check_start_pressed(keyevent, &current_input, pacman, pinkghost, redghost, blueghost, orangeghost);

            // TODO : ADD SOUND1
        }
        else if(game_state == GAME_OVER){
            tft_fill_screen(BLACK);

            draw_end_screen();

            // TODO : add sound 3
        }

        // TODO : Add idle state where we wait for the first input to start the game

        else if(game_state == GAMEPLAY){

        // Update Pacman pos
        update_pacman(current_input, &pacman);

        // Update Scoreboard
        update_scoreboard(&pacman, &scoreboard, redghost, orangeghost, pinkghost, blueghost);
        
        // TODO : Update Ghost Pos

        // Draw Pacman updated pos
        draw_pacman(current_input, pacman);

        // Draw Ghost updated pos
        draw_ghost(redghost, pacman);
        draw_ghost(blueghost, pacman);
        draw_ghost(pinkghost, pacman);
        draw_ghost(orangeghost, pacman);

        // Check pacman collision w/ ghosts
        game_state = check_collision(pacman, redghost, pinkghost, blueghost, orangeghost, &scoreboard);

        // TODO : Add sound 2

        sleep_ms(100); // Must wait so pacman doesnt move like hes on crack

        printf("SCORE: %d\n", scoreboard.score);
        printf("PELLETS: %d\n", scoreboard.num_pellets);
        printf("POWERS: %d\n", scoreboard.num_powers);
        }
    }
}

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display.h"
#include "controls.h"


// Controls Struct (for testing rn)
InputState current_input = {
    .joystick = INPUT_DIRECTION_LEFT,
    .start_pressed = true,
    .select_pressed = false
};

PacmanState pacman = {
    .y = 23,
    .x = 13,
    .lasty = 23,
    .lastx = 13,
    .direction = FACING_RIGHT
};

int main(){
    
    stdio_init_all();
    
    // Initialize the display and draw the map
    display_init();
    tft_fill_screen(BLACK);
    sleep_ms(100);
    
    // Draw Map and Initial Pacman 
    draw_map();
    draw_pacman(current_input, pacman);
    
    for(;;){

        // Update Pacman pos
        update_pacman(current_input, &pacman);

        // Draw Pacman updated pos
        draw_pacman(current_input, pacman);

        sleep_ms(100); // Must wait so pacman doesnt move like hes on crack
    }
}

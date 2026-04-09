#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display.h"
#include "controls.h"

// Init X and Y pos / Velocity Direction
uint8_t pacman_ypos = 23;
uint8_t pacman_xpos = 13;

uint8_t pacman_yvel = 0;
uint8_t pacman_xvel = 0;

// Controls Struct (for testing rn)
InputState current_input = {
    .joystick = INPUT_DIRECTION_RIGHT,
    .start_pressed = true,
    .select_pressed = false
};

int main(){
    
    stdio_init_all();
    
    // Initialize the display and draw the map
    display_init();
    tft_fill_screen(BLACK);
    sleep_ms(100);
    
    draw_map();
    
    for(;;){

        // Update Pacman pos
        update_pacman(current_input, &pacman_xpos, &pacman_ypos);

        // Draw Pacman updated pos
        draw_pacman(current_input, pacman_xpos, pacman_ypos);

        sleep_ms(100); // Must wait so pacman doesnt move like hes on crack
    }
}

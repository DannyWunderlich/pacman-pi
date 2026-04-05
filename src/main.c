#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display.h"

int main(){
    
    stdio_init_all();
    
    // Initialize the display and draw the map
    display_init();
    tft_fill_screen(BLACK);
    sleep_ms(1000);
    draw_map();

    for(;;){

        // tft_write_tile(tile_2, 0, 0, 7,7);
        
    }
}

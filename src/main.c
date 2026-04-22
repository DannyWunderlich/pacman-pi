#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "display.h"
#include "controls.h"

InputState current_input = {
    .joystick = INPUT_DIRECTION_NONE,
    .start_pressed = false,
    .select_pressed = false
};

volatile PacmanState pacman = {
    .y = PACMAN_START_Y,  //23
    .x = PACMAN_START_X,  //13
    .lasty = PACMAN_START_Y,
    .lastx = PACMAN_START_X,
    .direction = FACING_RIGHT,
    .mode = NORMAL
};

volatile GhostState redghost = {
    .y = HOUSE_START_LEFT_Y,
    .x = HOUSE_START_LEFT_X,
    .lasty = HOUSE_START_LEFT_Y,
    .lastx = HOUSE_START_LEFT_X,
    .color = COLOR_RED,
    .location = IN_HOUSE,
    .unlock_counter = 6,
    .direction = 3
};

volatile GhostState pinkghost = {
    .y = HOUSE_START_RIGHT_Y,
    .x = HOUSE_START_RIGHT_X,
    .lasty = HOUSE_START_RIGHT_Y,
    .lastx = HOUSE_START_RIGHT_X,
    .color = COLOR_PINK,
    .location = IN_HOUSE,
    .unlock_counter = 6,
    .direction = 1
};

volatile ScoreBoard scoreboard = {
    .score = 0,
    .lives = 3,
    .num_pellets = 237,
    .num_powers = 4,
    .total_food = 241
};

struct repeating_timer ssd_timer;
static uint32_t game_speed_timer = 0;
GameState game_state = STARTING_MENU;

int main() {

    stdio_init_all();

    // Seed with the current microsecond count since boot
    srand(to_us_since_boot(get_absolute_time()));
    
    // Initialize the display and draw the map
    display_init();
    tft_fill_screen(BLACK);
    sleep_ms(100);

    // Initialize the seven-segment display and add scoring timer so we can display score and lives
    ssd_init_spi();
    add_repeating_timer_ms(2, ssd_timer_callback, NULL, &ssd_timer);

    // Initialize joystick and buttons
    controls_init();

    // Initialize the Chomper interrupt (mode for when you pick up a powerup)
    init_chomper_timer();

    // Initialize ghost cage unlock timer
    init_ghostunlock_timer();

    // Initialize the game map storing the pellet / powers data matrix
    reset_game_map();

    // Game loop
    for(;;){
        controls_update();
        current_input = controls_get();

        switch (game_state) {
            case STARTING_MENU:
                // SOUND: add menu music

                // Flash the start screen text every 500ms
                display_flash_text(game_state);
                
                // Check if start button is pressed to begin game
                if (current_input.start_pressed) {
                    scoreboard.lives = 3;
                    scoreboard.score = 0;
                    ssd_update_buffer(scoreboard);
                    reset_level(&pacman, &redghost, &pinkghost, &scoreboard);
                    game_state = GAMEPLAY;
                    // SOUND: add start sound
                }
                break;

            case GAMEPLAY:

                if (current_input.start_pressed) {
                    draw_paused_screen();
                    game_state = PAUSED;
                    break; // Skip the rest of the frame
                }

                if (time_us_32() - game_speed_timer > 100000) {
                    update_pacman(current_input, &pacman);

                    if (pacman.mode == CHOMPER && (is_overlapping(pacman.x, pacman.y, redghost.x, redghost.y) || is_overlapping(pacman.x, pacman.y, pinkghost.x, pinkghost.y))) {
                        GhostState* hit_ghost = is_overlapping(pacman.x, pacman.y, redghost.x, redghost.y) ? (GhostState*)&redghost : (GhostState*)&pinkghost;

                        int old_gx = hit_ghost->x;
                        int old_gy = hit_ghost->y;

                        scoreboard.score += 200;
                        draw_pacman(current_input, pacman);

                        uint16_t gx = hit_ghost->x * TILE_WIDTH + HORIZONTAL_OFFSET - (TILE_WIDTH / 2);
                        uint16_t gy = hit_ghost->y * TILE_HEIGHT - (TILE_HEIGHT / 2);
                        tft_fill_region(gx, gy, gx + 15, gy + 15, BLACK);
                        draw_200_label(hit_ghost->x, hit_ghost->y);

                        hit_ghost->x = (hit_ghost->color == COLOR_RED) ? HOUSE_START_LEFT_X : HOUSE_START_RIGHT_X;
                        hit_ghost->y = (hit_ghost->color == COLOR_RED) ? HOUSE_START_LEFT_Y : HOUSE_START_RIGHT_Y;
                        hit_ghost->lastx = hit_ghost->x;
                        hit_ghost->lasty = hit_ghost->y;
                        hit_ghost->location = IN_HOUSE;

                        sleep_ms(750); // hitstop effect

                        for (int i = -1; i <= 1; i++) {
                            for (int j = -1; j <= 3; j++) { 
                                redraw_tile(old_gx + j, old_gy + i);
                            }
                        }
                    }

                    update_scoreboard(&pacman, &scoreboard, redghost, pinkghost);

                    // Check if all pellets were eaten, and reset level if so
                    if (scoreboard.total_food == 0) {
                        // Brief pause on victory
                        sleep_ms(1500); 

                        reset_level(&pacman, &redghost, &pinkghost, &scoreboard);                        
                        continue;
                    }

                    // Conditionally redraw black in house only when ghosts exit
                    static GhostLocation redghost_prev_location = IN_HOUSE;
                    static GhostLocation pinkghost_prev_location = IN_HOUSE;
                    if ((redghost.location == OUT_HOUSE) && (redghost_prev_location == IN_HOUSE))
                        redraw_black_in_house(redghost);
                    if ((pinkghost.location == OUT_HOUSE) && (pinkghost_prev_location == IN_HOUSE))
                        redraw_black_in_house(pinkghost);
                    redghost_prev_location = redghost.location;
                    pinkghost_prev_location = pinkghost.location;

                    update_ghost(&redghost, pacman);
                    update_ghost(&pinkghost, pacman);

                    draw_pacman(current_input, pacman);
                    draw_ghost(redghost, pacman);
                    draw_ghost(pinkghost, pacman);

                    if (check_collision(&pacman, &redghost, &pinkghost)) {
                        sleep_ms(750); // hitstop effect
                        scoreboard.lives -= 1;
                        ssd_update_buffer(scoreboard);
                        if (scoreboard.lives == 0) {
                            tft_fill_screen(BLACK);
                            game_state = GAME_OVER;
                        } else {
                            reset_sprites(&pacman, &redghost, &pinkghost);
                        }
                    }
                    game_speed_timer = time_us_32();
                }
                break;
            case PAUSED:
                // Check if start button is pressed to resume game
                if (current_input.start_pressed) {
                    
                    // Write "PAUSED" onto the screen and redraw sprites
                    draw_map();
                    draw_pacman(current_input, pacman);
                    draw_ghost(redghost, pacman);
                    draw_ghost(pinkghost, pacman);
                    
                    // Reset timer when unpausing 
                    game_speed_timer = time_us_32();
                    
                    game_state = GAMEPLAY;
                }
                break;
            case GAME_OVER:
                // SOUND: add game over sound

                // Flash the game over screen text every 500ms
                display_flash_text(game_state);

                // Check if start button is pressed to restart game
                if (current_input.start_pressed) {
                    tft_fill_screen(BLACK);
                    game_state = STARTING_MENU;
                }
                break;
        }
    }
}

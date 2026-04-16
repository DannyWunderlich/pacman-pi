#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "controls.h"

// TFT DISPLAY
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define TFT_SPI_CSN 17
#define TFT_SPI_SCK 18
#define TFT_SPI_MOSI 19

#define TFT_DC 20
#define TFT_RST 21

// SPRITE / TILES
#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define NUM_TILES_X 28
#define NUM_TILES_Y 31
#define NUM_PACMAN_TILES_X 2
#define NUM_PACMAN_TILES_Y 2
#define HORIZONTAL_OFFSET 48
#define NUM_GHOST_TILES_X 2
#define NUM_GHOST_TILES_Y 3

// Colors
#define BLACK ((uint16_t)0x0000)
#define BLUE ((uint16_t)0x001F)
#define YELLOW ((uint16_t)0xFFE0)
#define ORANGE ((uint16_t)0xFD86)
#define RED ((uint16_t)0xF800)
#define WHITE ((uint16_t)0xFFFF)
#define PINK ((uint16_t)0xF59A)
#define LIGHTBLUE ((uint16_t)0xA51E)

// Pacman Starting Location
#define PACMAN_START_X 13
#define PACMAN_START_Y 23

// Ghost Starting Locations
#define HOUSE_START_LEFT_Y 14
#define HOUSE_START_LEFT_X 12
#define HOUSE_START_RIGHT_Y 14
#define HOUSE_START_RIGHT_X 15

#define OUT_START_LEFT_Y 11
#define OUT_START_LEFT_X 9
#define OUT_START_RIGHT_Y 11
#define OUT_START_RIGHT_X 18

typedef enum{
    STARTING_MENU,
    GAMEPLAY,
    GAME_OVER
}GameState;

typedef enum{
    NORMAL,
    CHOMPER
}PacmanMode;

typedef enum {
    FACING_UP,
    FACING_LEFT,
    FACING_RIGHT,
    FACING_DOWN
}PacmanFacing;

typedef enum{
    COLOR_PINK,
    COLOR_RED,
    COLOR_BLUE,
    COLOR_ORANGE
}GhostColor;

typedef enum{
    IN_HOUSE,
    OUT_HOUSE
}GhostLocation;

typedef struct{
    uint8_t y;
    uint8_t x;
    uint8_t lasty;
    uint8_t lastx;
    PacmanFacing direction;
    PacmanMode mode;
}PacmanState;

typedef struct{
    uint8_t y;
    uint8_t x;
    uint8_t lasty;
    uint8_t lastx;
    GhostColor color;
    GhostLocation location;
    uint8_t unlock_counter;
    int direction; // (0: UP, 1: LEFT, 2: DOWN, 3: RIGHT)
}GhostState;

typedef struct{
    int score;
    uint8_t lives;
    uint8_t num_pellets;
    uint8_t num_powers;
    uint8_t total_food;
}ScoreBoard;

// Display Functions
void tft_write_command(uint8_t cmd);
void tft_write_data(uint8_t data);
void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void tft_fill_screen(uint16_t color);
void display_init(void);
void tft_write_tile(const uint16_t* tile, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void draw_map(void);
void draw_letter(const uint16_t* tile, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void draw_start_screen();
GameState check_start_pressed(uint16_t keyevent, InputState* current_input, PacmanState pacman, GhostState redghost, GhostState pinkghost);
GameState check_collision(PacmanState* pacman, GhostState* redghost, GhostState* pinkghost, ScoreBoard* scoreboard);
void draw_end_screen();

// Pacman Functions
void update_pacman(InputState controls, PacmanState* pacman);
void draw_pacman(InputState controls, PacmanState pacman);

// Ghost Functions
void update_ghost(GhostState* ghost, PacmanState pacman);
void draw_ghost(GhostState ghost, PacmanState pacman);
void init_ghostunlock_timer();
void ghostunlock_isr();

// Scoreboarding Functions
void update_scoreboard(PacmanState* pacman, ScoreBoard* scoreboard, GhostState redghost, GhostState pinkghost);
void init_chomper_timer();
void chomper_isr();

// Default map
extern uint8_t tile_map[NUM_TILES_Y][NUM_TILES_X];

// Global Pacman, Ghost, and scoreboard
extern volatile PacmanState pacman;
extern volatile ScoreBoard scoreboard;
extern volatile GhostState redghost;
// extern volatile GhostState orangeghost;
extern volatile GhostState pinkghost;
// extern volatile GhostState blueghost;

#endif
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

// Colors
#define BLACK ((uint16_t)0x0000)
#define BLUE ((uint16_t)0x001F)
#define YELLOW ((uint16_t)0xFFE0)

// Display Functions
void tft_write_command(uint8_t cmd);
void tft_write_data(uint8_t data);
void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void tft_fill_screen(uint16_t color);
void display_init(void);
void tft_write_tile(const uint16_t* tile, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void draw_map(void);

void update_pacman(InputState controls, uint8_t* pacman_x, uint8_t* pacman_y);
void draw_pacman(InputState controls, uint8_t pacman_x, uint8_t pacman_y);

// Tile Sprites
extern const uint16_t tile_1[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_2[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_3[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_4[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_5[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_6[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_7[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_8[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_9[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_10[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_11[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_12[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_13[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_14[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_15[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_16[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_17[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_18[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_19[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_20[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_21[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_22[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_23[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_24[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_25[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_26[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_27[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_28[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_29[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_30[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_31[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_32[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_33[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_34[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_35[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_36[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_37[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_38[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_39[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_40[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_41[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_42[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_43[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_44[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_45[TILE_HEIGHT * TILE_WIDTH];

// Pacman Sprites


// Default map
extern const uint8_t tile_map[NUM_TILES_Y][NUM_TILES_X];

#endif
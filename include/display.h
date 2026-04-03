#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

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

#define BLACK ((uint16_t)0x0000)
#define BLUE ((uint16_t)0x001F)

void tft_write_command(uint8_t cmd);
void tft_write_data(uint8_t data);
void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void tft_fill_screen(uint16_t color);
void display_init(void);
void tft_write_tile(const uint16_t* tile, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void draw_map(void);

extern const uint16_t tile_1[TILE_HEIGHT * TILE_WIDTH];
extern const uint16_t tile_2[TILE_HEIGHT * TILE_WIDTH];

#endif
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "display.h"

void tft_write_command(uint8_t cmd){
    gpio_put(TFT_DC, 0); // Command mode
    gpio_put(TFT_SPI_CSN, 0);
    spi_write_blocking(spi0, &cmd, 1);
    gpio_put(TFT_SPI_CSN, 1);
}

void tft_write_data(uint8_t data){
    gpio_put(TFT_DC, 1); // Data mode
    gpio_put(TFT_SPI_CSN, 0);
    spi_write_blocking(spi0, &data, 1);
    gpio_put(TFT_SPI_CSN, 1);
}

void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column Address Set
    tft_write_command(0x2A);
    tft_write_data(x0 >> 8);
    tft_write_data(x0 & 0xFF);
    tft_write_data(x1 >> 8);
    tft_write_data(x1 & 0xFF);

    // Row Address Set
    tft_write_command(0x2B);
    tft_write_data(y0 >> 8);
    tft_write_data(y0 & 0xFF);
    tft_write_data(y1 >> 8);
    tft_write_data(y1 & 0xFF);

    // Write to RAM
    tft_write_command(0x2C);
}

void tft_fill_screen(uint16_t color) {
    tft_set_address_window(0, 0, 319, 239);

    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);

    gpio_put(TFT_DC, 1);
    gpio_put(TFT_SPI_CSN, 0);

    // Loop through every pixel on the screen (240 * 320)
    for (uint32_t i = 0; i < (240 * 320); i++) {
        // Send high byte then low byte <--- ILI9341 IS BIG ENDIAN
        spi_write_blocking(spi0, &hi, 1);
        spi_write_blocking(spi0, &lo, 1);
    }

    gpio_put(TFT_SPI_CSN, 1);
}

void tft_write_tile(const uint16_t* tile, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    tft_set_address_window(x0, y0, x1, y1);

    gpio_put(TFT_DC, 1);
    gpio_put(TFT_SPI_CSN, 0);

    for(int i = 0; i < TILE_HEIGHT * TILE_WIDTH; i ++){
        uint8_t hi = (uint8_t)(tile[i] >> 8);
        uint8_t lo = (uint8_t)(tile[i] & 0xFF);

        spi_write_blocking(spi0, &hi, 1);
        spi_write_blocking(spi0, &lo, 1);
    }

    gpio_put(TFT_SPI_CSN, 1);
}

void draw_map(){
    for(int i = 0; i < NUM_TILES_Y; i ++){
        for(int j = 0; j < NUM_TILES_X; j ++){
                
            uint16_t x0 = j * TILE_WIDTH;
            uint16_t y0 = i * TILE_HEIGHT;
            uint16_t x1 = x0 + TILE_WIDTH - 1;
            uint16_t y1 = y0 + TILE_HEIGHT - 1;

            tft_write_tile(tile_1, x0, y0, x1, y1);
            // sleep_ms(5);
        }
    }
}

void display_init(){
    
    // Initialize Pins and SPI
    gpio_init(TFT_SPI_SCK);
    gpio_init(TFT_SPI_MOSI);

    gpio_set_function(TFT_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(TFT_SPI_MOSI, GPIO_FUNC_SPI);

    gpio_init(TFT_SPI_CSN);
    gpio_init(TFT_DC);
    gpio_init(TFT_RST);

    gpio_set_dir(TFT_SPI_CSN, GPIO_OUT);
    gpio_set_dir(TFT_DC, GPIO_OUT);
    gpio_set_dir(TFT_RST, GPIO_OUT);

    spi_init(spi0, 16000000);

    // Reset Display
    gpio_put(TFT_RST, 0);
    sleep_ms(10);
    gpio_put(TFT_RST, 1);
    sleep_ms(10);

    // Basic Initialization Sequence
    tft_write_command(0x01); // Software Reset
    sleep_ms(100);
    tft_write_command(0x11); // Exit Sleep
    sleep_ms(100);

    // Set Pixel Format to 16-bit (RGB565)
    tft_write_command(0x3A);
    tft_write_data(0x55);
    sleep_ms(100);

    // Using Landscape mode. x goes to 319 and y goes to 239 (320x249)
    tft_write_command(0x36);
    tft_write_data(0x28);

    tft_write_command(0x29); // Display ON
}


#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "controls.h"

/* Joystick (ADC) parameters */
static const uint16_t JOYSTICK_CENTER_X = 2048;
static const uint16_t JOYSTICK_CENTER_Y = 2048;
static const uint16_t JOYSTICK_DEADZONE = 400;
static const uint16_t JOYSTICK_TIEBREAKER = 20;
static const uint JOYSTICK_ADC_GPIO_X = 44;
static const uint JOYSTICK_ADC_GPIO_Y = 45;
static const uint JOYSTICK_ADC_CH_X = 4;
static const uint JOYSTICK_ADC_CH_Y = 5;

/* Pushbutton (I2C) parameters */
static i2c_inst_t* const I2C_BUTTONS = i2c1;
static const uint I2C_BUTTONS_SDA = 42;
static const uint I2C_BUTTONS_SCL = 43;
static const uint I2C_BUTTONS_BAUD = 100000;
static const uint8_t I2C_BUTTONS_START_BIT = 0;
static const uint8_t I2C_BUTTONS_SELECT_BIT = 1;

/* MCP23017 I/O expansion board register addresses */
static const uint8_t IO_BOARD_ADDR = 0x27; // Default address of the MCP23017
static const uint8_t IO_BOARD_DIR = 0x00; // Port A I/O pin directions register
static const uint8_t IO_BOARD_INV = 0x02; // Port A I/O pin invert input polarity register
static const uint8_t IO_BOARD_PU = 0x0C; // Port A I/O pin pull-up register
static const uint8_t IO_BOARD_GPIO = 0x12; // Port A I/O pin values register

/* Proton Board pushbutton (GPIO 26)*/
static const uint GPIO_BUTTON_START = 26;

bool gpio_button_previous;
static uint8_t i2c_buttons_previous;

static InputState controls;

void controls_init(void) {
    // Initialize ADC for joystick inputs X/Y (GPIO 44-45)
    adc_init();
    adc_gpio_init(JOYSTICK_ADC_GPIO_X);
    adc_gpio_init(JOYSTICK_ADC_GPIO_Y);

    // Initialize I2C for MCP23017 I/O expansion board (GPIO 42-43)
    i2c_init(I2C_BUTTONS, I2C_BUTTONS_BAUD);
    gpio_set_function(I2C_BUTTONS_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_BUTTONS_SCL, GPIO_FUNC_I2C);

    /*
    Configure MCP23017 I/O expansion board connected pushbuttons:
    - START and SELECT buttons connected to pins PA0 and PA1 
    - All port A pins are set as inputs, pulled-up, and inverted (pressed = 1)
    - Default I2C address of MCP23017 is 0x27
    */

    uint8_t i2c_write_data[2];

    // Set all port A I/O pins as inputs
    i2c_write_data[0] = IO_BOARD_DIR;
    i2c_write_data[1] = 0xFF;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false);

    // Enable pull-up resistors on all port A I/O pins 
    i2c_write_data[0] = IO_BOARD_PU;
    i2c_write_data[1] = 0xFF;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false);

    // Invert input polarity on all port A I/O pins
    i2c_write_data[0] = IO_BOARD_INV;
    i2c_write_data[1] = 0xFF;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false);

    /* Configure built-in pushbutton (GPIO 26) on top of the Proton board as extra START button
       Just for testing purposes if you don't have MCP23017 this will work too.*/
    gpio_init(GPIO_BUTTON_START);
    gpio_set_dir(GPIO_BUTTON_START, false);
    gpio_pull_down(GPIO_BUTTON_START);

    // Initialize control states
    controls.joystick = INPUT_DIRECTION_NONE;
    controls.start_pressed  = false;
    controls.select_pressed = false;

    gpio_button_previous = 0;
    i2c_buttons_previous = 0;
}

void controls_update(void) {
    controls.start_pressed = false;
    controls.select_pressed = false;

    /* Joystick (ADC) update logic */

    // Select and sample the ADC channel configured to X-axis pin (44)
    adc_select_input(JOYSTICK_ADC_CH_X);
    uint16_t vrx = (uint16_t)adc_read();

    // Select and sample  the ADC channel configured to Y-axis pin (45)
    adc_select_input(JOYSTICK_ADC_CH_Y);
    uint16_t vry = (uint16_t)adc_read();

    // Calculate distance of each sample from center
    int x_dist = abs((int)vrx - (int)JOYSTICK_CENTER_X);
    int y_dist = abs((int)vry - (int)JOYSTICK_CENTER_Y);

    // Determine joystick direction based off X/Y axes distances from center
    if (x_dist < (int)JOYSTICK_DEADZONE && y_dist < (int)JOYSTICK_DEADZONE) {
        controls.joystick = INPUT_DIRECTION_NONE;
    } else {
        switch (controls.joystick) {
            case INPUT_DIRECTION_LEFT:
            case INPUT_DIRECTION_RIGHT:
                if (y_dist > (x_dist + JOYSTICK_TIEBREAKER)) {
                    controls.joystick = (vry < JOYSTICK_CENTER_Y) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
                } else {
                    controls.joystick = (vrx < JOYSTICK_CENTER_X) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
                }
                break;
            case INPUT_DIRECTION_UP:
            case INPUT_DIRECTION_DOWN:
                if (x_dist > (y_dist + JOYSTICK_TIEBREAKER)) {
                    controls.joystick = (vrx < JOYSTICK_CENTER_X) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
                } else {
                    controls.joystick = (vry < JOYSTICK_CENTER_Y) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
                }
                break;
            default:
                if (x_dist >= y_dist) {
                    controls.joystick = (vrx < JOYSTICK_CENTER_X) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
                } else {
                    controls.joystick = (vry < JOYSTICK_CENTER_Y) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
                }
                break;
        }
    }

    /* Proton Board pushbutton update logic */

    bool gpio_button_current = gpio_get(GPIO_BUTTON_START);
    bool gpio_button_pressed = gpio_button_current && !gpio_button_previous;
    if (gpio_button_pressed) controls.start_pressed = true;
    gpio_button_previous = gpio_button_current;
    
    /* Pushbutton (I2C) update logic */

    uint8_t i2c_read_data;

    // Select pin values register (return early if error occurs)
    if (i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, &IO_BOARD_GPIO, 1, true) != 1) return;
    
    // Read pin values register data (return early if error occurs)
    if (i2c_read_blocking(I2C_BUTTONS, IO_BOARD_ADDR, &i2c_read_data, 1, false) != 1) return;

    uint8_t buttons_current = i2c_read_data; 

    // Flag START and/or SELECT bit as pressed if a rising edge was detected respectively
    uint8_t buttons_pressed = (uint8_t)(buttons_current & ~i2c_buttons_previous);
    if (buttons_pressed & (1u << I2C_BUTTONS_START_BIT)) controls.start_pressed = true;
    if (buttons_pressed & (1u << I2C_BUTTONS_SELECT_BIT)) controls.select_pressed = true;

    i2c_buttons_previous = buttons_current;
}

InputState controls_get(void) { return controls; }

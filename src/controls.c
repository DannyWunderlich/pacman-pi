#include "controls.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#include <stdlib.h>
#include <stdint.h>

/* Joystick (ADC) parameters */
static const uint16_t CENTER_X          = 2048; // 12-bit ADC center
static const uint16_t CENTER_Y          = 2048; // 12-bit ADC center
static const uint16_t DEADZONE          = 400;

static const uint ADC_GPIO_JOYSTICK_X   = 44;
static const uint ADC_GPIO_JOYSTICK_Y   = 45;
static const uint ADC_CH_JOYSTICK_X     = 4;
static const uint ADC_CH_JOYSTICK_Y     = 5;

/* Pushbuttons (I2C) parameters */
static i2c_inst_t* const I2C_BUTTONS    = i2c1;
static const uint I2C_SDA_PIN           = 42;
static const uint I2C_SCL_PIN           = 43;
static const uint I2C_BAUD_HZ           = 100000;

/* MCP23017 I/O expansion board parameters */
static const uint8_t IO_BOARD_ADDR      = 0x20;
static const uint8_t IO_BOARD_DIR       = 0x00; // 1 = pin is input
static const uint8_t IO_BOARD_INV       = 0x02; // 1 = invert input polarity
static const uint8_t IO_BOARD_PU        = 0x0C; // 1 = enable pull-up resistor
static const uint8_t IO_BOARD_GPIO      = 0x12; // Current pin values on port A

/* Button bit positions on MCP23017 port A */
static const uint8_t BUTTONS_START_BIT  = 0;
static const uint8_t BUTTONS_SELECT_BIT = 1;
static const uint8_t BUTTONS_MASK       = (uint8_t)((1u << BUTTONS_START_BIT) | (1u << BUTTONS_SELECT_BIT));

static uint8_t i2c_read_data;
static uint8_t i2c_write_data[2];

static uint8_t buttons_previous;
static uint8_t buttons_current;
static uint8_t buttons_pressed;

static InputState controls;

void controls_init(void) {
    /* Initialize ADC for joystick inputs X/Y (GPIO 44-45) */
    adc_init();
    adc_gpio_init(ADC_GPIO_JOYSTICK_X);
    adc_gpio_init(ADC_GPIO_JOYSTICK_Y);

    /* Initialize I2C for MCP23017 I/O expansion board (GPIO 42-43) */
    i2c_init(I2C_BUTTONS, I2C_BAUD_HZ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    /* Configure MCP23017 I/O pushbutton (pins 0-1) */
    i2c_write_data[0] = IO_BOARD_DIR;
    i2c_write_data[1] = BUTTONS_MASK;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false); // Set pins 0-1 as inputs

    i2c_write_data[0] = IO_BOARD_PU;
    i2c_write_data[1] = BUTTONS_MASK;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false); // Enable pull-up resistors on pins 0-1

    i2c_write_data[0] = IO_BOARD_INV;
    i2c_write_data[1] = BUTTONS_MASK;
    i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, i2c_write_data, 2, false); // Invert input polarity of pins 0-1 so they are active-high when pressed

    /* Set inputs as default values */
    controls.joystick       = INPUT_DIRECTION_NONE;
    controls.start_pressed  = false;
    controls.select_pressed = false;
    buttons_previous        = 0;
    buttons_current         = 0;
    buttons_pressed         = 0;
}

void controls_update(void) {
    controls.start_pressed = false;
    controls.select_pressed = false;

    /* Joystick update logic */

    // Call adc_read twice to allow channel selection to settle
    adc_select_input(ADC_CH_JOYSTICK_X);
    (void)adc_read();
    uint16_t vrx = (uint16_t)adc_read();

    adc_select_input(ADC_CH_JOYSTICK_Y);
    (void)adc_read();
    uint16_t vry = (uint16_t)adc_read();

    // Calculate distance from center for both axes
    int x_dist = abs((int)vrx - (int)CENTER_X);
    int y_dist = abs((int)vry - (int)CENTER_Y);

    // If both axes are inside deadzone do nothing
    // Otherwise check which axis is pushed further (x axis is chosen if distances are equal)
    // and set direction corresponding to if chosen axis moved in positive or negative direction
    if (x_dist < (int)DEADZONE && y_dist < (int)DEADZONE) {
        controls.joystick = INPUT_DIRECTION_NONE;
    } else if (x_dist >= y_dist) {
        controls.joystick = (vrx < CENTER_X) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
    } else {
        controls.joystick = (vry < CENTER_Y) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
    }

    /* Pushbutton update logic */

    // Select the MCP23017 port A pin values register (0x12) by writing the address via I2C
    if (i2c_write_blocking(I2C_BUTTONS, IO_BOARD_ADDR, &IO_BOARD_GPIO, 1, true) != 1) {
        return;
    }
    // Read from the selected pin values register
    if (i2c_read_blocking(I2C_BUTTONS, IO_BOARD_ADDR, &i2c_read_data, 1, false) != 1) {
        return;
    }

    // Mask the read data for only pushbutton pins 0-1
    buttons_current = (uint8_t)(i2c_read_data & BUTTONS_MASK);

    // Detect if pins 0-1 had a rising edge
    buttons_pressed = (uint8_t)(buttons_current & ~buttons_previous);

    if (buttons_pressed & (1u << BUTTONS_START_BIT)) {
        controls.start_pressed = true;
    }
    if (buttons_pressed & (1u << BUTTONS_SELECT_BIT)) {
        controls.select_pressed = true;
    }

    buttons_previous = buttons_current;
}

InputState controls_get(void) {
    return controls;
}

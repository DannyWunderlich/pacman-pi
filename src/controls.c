#include "controls.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#include <stdlib.h>
#include <stdint.h>

static const uint16_t CENTER = 2048;
static const uint16_t DEADZONE = 400;

static const uint ADC_GPIO_JOYSTICK_X = 44;
static const uint ADC_GPIO_JOYSTICK_Y = 45;
static const uint ADC_CH_JOYSTICK_X = 4;
static const uint ADC_CH_JOYSTICK_Y = 5;

static i2c_inst_t* const I2C_BUTTONS = i2c0;
static const uint I2C_SDA_PIN = 42;
static const uint I2C_SCL_PIN = 43;

static InputState controls;

void controls_init(void) {
    adc_init();
    adc_gpio_init(ADC_GPIO_JOYSTICK_X);
    adc_gpio_init(ADC_GPIO_JOYSTICK_Y);

    i2c_init(I2C_BUTTONS, 100000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    controls.joystick = INPUT_DIRECTION_NONE;
    controls.start_pressed = false;
    controls.select_pressed = false;
}

void controls_update(void) {
    controls.start_pressed = false;
    controls.select_pressed = false;

    /* Joystick decode */
    adc_select_input(ADC_CH_JOYSTICK_X);
    uint16_t vrx = adc_read();

    adc_select_input(ADC_CH_JOYSTICK_Y);
    uint16_t vry = adc_read();

    int x_dist = abs((int)vrx - (int)CENTER);
    int y_dist = abs((int)vry - (int)CENTER);

    if (x_dist < (int)DEADZONE && y_dist < (int)DEADZONE) {
        controls.joystick = INPUT_DIRECTION_NONE;
    }
    else if (x_dist > y_dist) {
        controls.joystick = (vrx > CENTER) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
    }
    else {
        controls.joystick = (vry > CENTER) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
    }

    /* Pushbutton reads + debounce */

}

InputState controls_get(void) {
    return controls;
}

// #include "pico/stdlib.h"
// #include "hardware/adc.h"
// #include "hardware/i2c.h"

// #include <stdlib.h>
// #include <stdint.h>
// #include <stdbool.h>

// typedef enum {
//     INPUT_DIRECTION_NONE,
//     INPUT_DIRECTION_UP,
//     INPUT_DIRECTION_DOWN,
//     INPUT_DIRECTION_LEFT,
//     INPUT_DIRECTION_RIGHT
// } InputDirection;

// typedef struct {
//     InputDirection joystick;
//     bool start_pressed;
//     bool select_pressed;
// } InputState;

// static const uint16_t CENTER = 2048;
// static const uint16_t DEADZONE = 400;

// static const uint ADC_GPIO_JOYSTICK_X = 44;
// static const uint ADC_GPIO_JOYSTICK_Y = 45;
// static const uint ADC_CH_JOYSTICK_X = 4;
// static const uint ADC_CH_JOYSTICK_Y = 5;

// static i2c_inst_t* const I2C_BUTTONS = i2c1;
// static const uint I2C_SDA_PIN = 42;
// static const uint I2C_SCL_PIN = 43;
// static const uint I2C_BAUD_HZ = 100000;

// // MCP23017 I/O expander
// static const uint8_t MCP23017_ADDR = 0x20;
// static const uint8_t MCP_IODIR = 0x00;  // 1 = pin is input
// static const uint8_t MCP_INV   = 0x02;  // 1 = invert input polarity
// static const uint8_t MCP_PU    = 0x0C;  // 1 = enable pull-up resistor
// static const uint8_t MCP_GPIO  = 0x12;  // current pin values on port A

// // Buttons wired to GND on port A pins 0 and 1.
// static const uint8_t BTN_START_BIT  = 0;
// static const uint8_t BTN_SELECT_BIT = 1;
// static const uint8_t BTN_MASK = (uint8_t)((1u << BTN_START_BIT) | (1u << BTN_SELECT_BIT));

// static InputState controls;
// static uint8_t prev_buttons;

// static bool mcp_write_reg(uint8_t reg, uint8_t value) {
//     uint8_t buf[2] = { reg, value };
//     int rc = i2c_write_blocking(I2C_BUTTONS, MCP23017_ADDR, buf, 2, false);
//     return rc == 2;
// }

// static bool mcp_read_reg(uint8_t reg, uint8_t *out) {
//     int rc = i2c_write_blocking(I2C_BUTTONS, MCP23017_ADDR, &reg, 1, true);
//     if (rc != 1) return false;
//     rc = i2c_read_blocking(I2C_BUTTONS, MCP23017_ADDR, out, 1, false);
//     return rc == 1;
// }

// void controls_init(void) {
//     adc_init();
//     adc_gpio_init(ADC_GPIO_JOYSTICK_X);
//     adc_gpio_init(ADC_GPIO_JOYSTICK_Y);

//     i2c_init(I2C_BUTTONS, I2C_BAUD_HZ);
//     gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
//     gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
//     gpio_pull_up(I2C_SDA_PIN);
//     gpio_pull_up(I2C_SCL_PIN);

//     mcp_write_reg(MCP_IODIR, BTN_MASK); // set I2C buttons as input
//     mcp_write_reg(MCP_PU, BTN_MASK); // enable pull-up resistor
//     mcp_write_reg(MCP_INV, BTN_MASK); // invert button press to read as 1

//     controls.joystick = INPUT_DIRECTION_NONE;
//     controls.start_pressed = false;
//     controls.select_pressed = false;
//     prev_buttons = 0;
// }

// void controls_update(void) {
//     controls.start_pressed = false;
//     controls.select_pressed = false;

//     /* Joystick decode */
//     adc_select_input(ADC_CH_JOYSTICK_X);
//     uint16_t vrx = adc_read();

//     adc_select_input(ADC_CH_JOYSTICK_Y);
//     uint16_t vry = adc_read();

//     int x_dist = abs((int)vrx - (int)CENTER);
//     int y_dist = abs((int)vry - (int)CENTER);

//     if (x_dist < (int)DEADZONE && y_dist < (int)DEADZONE) {
//         controls.joystick = INPUT_DIRECTION_NONE;
//     }
//     else if (x_dist > y_dist) {
//         controls.joystick = (vrx > CENTER) ? INPUT_DIRECTION_LEFT : INPUT_DIRECTION_RIGHT;
//     }
//     else {
//         controls.joystick = (vry > CENTER) ? INPUT_DIRECTION_UP : INPUT_DIRECTION_DOWN;
//     }

//     /* Pushbutton reads + debounce */
//     uint8_t gpio = 0;
//     if (!mcp_read_reg(MCP_GPIO, &gpio)) {
//         return; // on I2C read failure, skip buttons this tick
//     }
//     // Pulse corresponding button when pressed
//     uint8_t buttons = (uint8_t)(gpio & BTN_MASK);
//     uint8_t just_pressed = (uint8_t)(buttons & ~prev_buttons);

//     if (just_pressed & (1u << BTN_START_BIT))  controls.start_pressed  = true;
//     if (just_pressed & (1u << BTN_SELECT_BIT)) controls.select_pressed = true;

//     prev_buttons = buttons;
// }

// InputState controls_get(void) {
//     return controls;
// }

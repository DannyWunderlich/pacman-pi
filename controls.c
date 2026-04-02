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

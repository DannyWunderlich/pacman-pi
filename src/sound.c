#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "sound.h"

#define SOUND_GPIO 36
#define DUTY_PERCENT 50

typedef enum {
    SOUND_IDLE,
    SOUND_MENU_BEEP,
    SOUND_START,
    SOUND_GAMEPLAY_BEEP,
    SOUND_GAMEOVER
} SoundMode;

static uint sound_slice;
static uint sound_channel;

static SoundMode sound_mode = SOUND_IDLE;

static bool gameplay_enabled = false;
static bool menu_enabled = false;

static absolute_time_t one_shot_end_time;
static absolute_time_t beep_toggle_time;
static bool beep_on = false;

static void sound_set_pwm_freq(uint32_t freq_hz) {
    if (freq_hz == 0) {
        pwm_set_chan_level(sound_slice, sound_channel, 0);
        return;
    }

    uint32_t sys_clk = clock_get_hz(clk_sys);

    float clkdiv = 1.0f;
    uint32_t wrap = (uint32_t)(sys_clk / freq_hz) - 1u;

    while (wrap > 65535u && clkdiv < 255.0f) {
        clkdiv += 1.0f;
        wrap = (uint32_t)(sys_clk / (clkdiv * freq_hz)) - 1u;
    }

    if (wrap > 65535u) wrap = 65535u;
    if (wrap < 10u) wrap = 10u;

    pwm_set_clkdiv(sound_slice, clkdiv);
    pwm_set_wrap(sound_slice, (uint16_t)wrap);

    uint16_t level = (uint16_t)((wrap * DUTY_PERCENT) / 100u);
    pwm_set_chan_level(sound_slice, sound_channel, level);
}

static void sound_output_off(void) {
    sound_set_pwm_freq(0);
}

void sound_init(void) {
    gpio_set_function(SOUND_GPIO, GPIO_FUNC_PWM);

    sound_slice = pwm_gpio_to_slice_num(SOUND_GPIO);
    sound_channel = pwm_gpio_to_channel(SOUND_GPIO);

    pwm_set_clkdiv(sound_slice, 1.0f);
    pwm_set_wrap(sound_slice, 1000);
    pwm_set_chan_level(sound_slice, sound_channel, 0);
    pwm_set_enabled(sound_slice, true);

    gameplay_enabled = false;
    menu_enabled = false;
    sound_mode = SOUND_IDLE;
    beep_on = false;
}

void sound_update(void) {
    switch (sound_mode) {
        case SOUND_START:
        case SOUND_GAMEOVER:
            if (absolute_time_diff_us(get_absolute_time(), one_shot_end_time) <= 0) {
                if (gameplay_enabled) {
                    sound_mode = SOUND_GAMEPLAY_BEEP;
                    beep_on = true;
                    sound_set_pwm_freq(440);
                    beep_toggle_time = make_timeout_time_ms(120);
                } else if (menu_enabled) {
                    sound_mode = SOUND_MENU_BEEP;
                    beep_on = true;
                    sound_set_pwm_freq(220);
                    beep_toggle_time = make_timeout_time_ms(250);
                } else {
                    sound_mode = SOUND_IDLE;
                    sound_output_off();
                }
            }
            break;

        case SOUND_MENU_BEEP:
            if (!menu_enabled) {
                sound_mode = SOUND_IDLE;
                beep_on = false;
                sound_output_off();
                break;
            }

            if (absolute_time_diff_us(get_absolute_time(), beep_toggle_time) <= 0) {
                if (beep_on) {
                    sound_output_off();
                    beep_on = false;
                    beep_toggle_time = make_timeout_time_ms(500);
                } else {
                    sound_set_pwm_freq(220);
                    beep_on = true;
                    beep_toggle_time = make_timeout_time_ms(180);
                }
            }
            break;

        case SOUND_GAMEPLAY_BEEP:
            if (!gameplay_enabled) {
                sound_mode = SOUND_IDLE;
                beep_on = false;
                sound_output_off();
                break;
            }

            if (absolute_time_diff_us(get_absolute_time(), beep_toggle_time) <= 0) {
                if (beep_on) {
                    sound_output_off();
                    beep_on = false;
                    beep_toggle_time = make_timeout_time_ms(90);
                } else {
                    sound_set_pwm_freq(440);
                    beep_on = true;
                    beep_toggle_time = make_timeout_time_ms(90);
                }
            }
            break;

        case SOUND_IDLE:
        default:
            break;
    }
}

void sound_play_start(void) {
    menu_enabled = false;
    sound_mode = SOUND_START;
    sound_set_pwm_freq(880);
    one_shot_end_time = make_timeout_time_ms(250);
}

void sound_set_gameplay_active(bool active) {
    gameplay_enabled = active;

    if (active) {
        menu_enabled = false;
        sound_mode = SOUND_GAMEPLAY_BEEP;
        beep_on = true;
        sound_set_pwm_freq(440);
        beep_toggle_time = make_timeout_time_ms(90);
    } else {
        if (sound_mode == SOUND_GAMEPLAY_BEEP) {
            sound_mode = SOUND_IDLE;
            beep_on = false;
            sound_output_off();
        }
    }
}

void sound_set_menu_active(bool active) {
    menu_enabled = active;

    if (active) {
        gameplay_enabled = false;
        sound_mode = SOUND_MENU_BEEP;
        beep_on = true;
        sound_set_pwm_freq(220);
        beep_toggle_time = make_timeout_time_ms(180);
    } else {
        if (sound_mode == SOUND_MENU_BEEP) {
            sound_mode = SOUND_IDLE;
            beep_on = false;
            sound_output_off();
        }
    }
}

void sound_play_game_over(void) {
    gameplay_enabled = false;
    menu_enabled = false;
    sound_mode = SOUND_GAMEOVER;
    sound_set_pwm_freq(180);
    one_shot_end_time = make_timeout_time_ms(700);
}

void sound_stop(void) {
    gameplay_enabled = false;
    menu_enabled = false;
    sound_mode = SOUND_IDLE;
    beep_on = false;
    sound_output_off();
}
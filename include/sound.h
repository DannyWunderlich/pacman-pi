#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>

void sound_init(void);
void sound_update(void);

void sound_play_start(void);
void sound_set_gameplay_active(bool active);
void sound_play_game_over(void);
void sound_stop(void);
void sound_set_menu_active(bool active);

#endif
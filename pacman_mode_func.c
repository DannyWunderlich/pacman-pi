int pacman_mode_func(float pacman_loc[2], int map[row][columns], float *powerup_timer, int pacman_mode)
{
    int powerup_time = 8;//change to define
    float tick_value = 1; // change to define
    // 1 = normal
    // 2 = powerup
    //powerup_time represents how long pacman stays powered up after eating powerup
    //3 on map represents powerup, 2 = free space
    if (map[floor(pacman_loc[0])][floor(pacman_loc[1])] == 3)
    {
        map[floor(pacman_loc[0])][floor(pacman_loc[1])] = 2;
        *powerup_timer = powerup_time;
    }
    else if (*powerup_timer >= 0)
    {
        *powerup_timer -= tick_value;
    }
    if (*powerup_timer > 0)
    {
        pacman_mode = 2;
    }
    else
    {
        pacman_mode = 1;
    }
    return pacman_mode;
}

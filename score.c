int score(int map[rows][columns], float pacman_loc[2], float blinky_loc[2], float inky_loc[2], float pinky_loc[2], float clyde_loc[2], int pacman_mode, int score)
{
    // tracks score and updates map
    // 1 represents dot on map, 2 represents empty
    // pacman_mode 1 = normal, 2 = powerup
    int dot_value = 100;
    int ghost_value = 200;
    if (pacman_mode == 2)
    {
        if (fabs(pacman_loc[0] - blinky_loc[0]) < .25 && fabs(pacman_loc[1] - blinky_loc[1]) < .25)
        {
            score += ghost_value;
        }
        if (fabs(pacman_loc[0] - inky_loc[0]) < .25 && fabs(pacman_loc[1] - inky_loc[1]) < .25)
        {
            score += ghost_value;
        }
        if (fabs(pacman_loc[0] - pinky_loc[0]) < .25 && fabs(pacman_loc[1] - pinky_loc[1]) < .25)
        {
            score += ghost_value;
        }
        if (fabs(pacman_loc[0] - clyde_loc[0]) < .25 && fabs(pacman_loc[1] - clyde_loc[1]) < .25)
        {
            score += ghost_value;
        }
    }
    if (map[floor(pacman_loc[0])][floor(pacman_loc[1])] == 1)
    {
        score += dot_value;
        map[floor(pacman_loc[0])][floor(pacman_loc[1])] = 2;
    }
    return score;
}

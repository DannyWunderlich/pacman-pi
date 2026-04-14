int ghost_mmode_func(int pacman_mode, int map[row][column], int ghost_mode, int scatter_target[2], int ghost_loc[2])
{
    // pacman mode 1 = normal, 2 = powerup
    // ghost mode 1 = scatter 2 = chase 3 = scared
    // blinky heads to upper right in scatter
    // inky lower right corner
    // pinky upper left
    // clyde lower left
    if (pacman_mode == 2)
    {
        ghost_mode = 3;
    }
    else if (ghost_mode == 1)
    {
        if (ghost_loc[0] == scatter_target[0] && ghost_loc[1] == scatter_target[1])
        {
            ghost_mode = 2;
        }
        else
        {
            ghost_mode = 1;
        }
    }
    else 
    {
        ghost_mode = 2;
    }
    return ghost_mode;
}

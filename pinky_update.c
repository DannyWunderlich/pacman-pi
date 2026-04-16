void pinky_update(int pinky_loc[2], int pacman_loc[2], int map[NUM_TILES_Y][NUM_TILES_X], int pacman_mode, int ghost_mode, int pacman_movement)
{
    // to do 1. test scatter mode
    // 2. add scared mode
    // 3. move towards pacman
    // 4. prevent oscilation
    // 5. set to reset postion if eaten
    
    // pacman movement 1 = up, 2 = down , 3 = right, 4 = left
    // find distance from desired position
    int distance_x; // direction blinky wants to move horizontally
    int distance_y; // direction blinky wants to move vertically
    if (pacman_mode == 2) // frightened mode
    {
        distance_y = pinky_loc[1] - pacman_loc[1];
        distance_x = pinky_loc[0] - pacman_loc[0];
    }
    else if (ghost_mode == 1) //  scatter mode
    {
        distance_y = NUM_TILES_X - 1 - pinky_loc[1];// change
        distance_x = 1 - pinky_loc[0];
    }
    else // chase mode
    {
        if (pacman_movement == 1)
        {
        distance_y = pacman_loc[1] - 2 - pinky_loc[1];
        distance_x = pacman_loc[0] - pinky_loc[0];
        }
        else if (pacman_movement == 2)
        {
            distance_y = pacman_loc[1] + 2 - pinky_loc[1];
            distance_x = pacman_loc[0] - pinky_loc[0];
        }
        else if (pacman_movement == 3)
        {
            distance_y = pacman_loc[1] - pinky_loc[1];
            distance_x = pacman_loc[0] + 2 - pinky_loc[0];
        }
                else if (pacman_movement == 2)
        {
            distance_y = pacman_loc[1] - pinky_loc[1];
            distance_x = pacman_loc[0] - 2 - pinky_loc[0];
        }
    }

    // find if blinky can move in each direction
    int moveup;
    int movedown;
    int moveright;
    int moveleft;
    if (map[pinky_loc[0] + 1][pinky_loc[1]] == 45 || map[pinky_loc[0] + 1][pinky_loc[1]] == 46 || map[pinky_loc[0] + 1][pinky_loc[1]] == 47)
    {
        moveright = 1;
    }
    else
    {
        moveright = 0;
    }
    if (map[pinky_loc[0] - 1][pinky_loc[1]] == 45 || map[pinky_loc[0] - 1][pinky_loc[1]] == 46 || map[pinky_loc[0] - 1][pinky_loc[1]] == 47)
    {
        moveleft = 1;
    }
    else
    {
        moveleft = 0;
    }
    if (map[pinky_loc[0]][pinky_loc[1] - 1] == 45 || map[pinky_loc[0]][pinky_loc[1] - 1] == 46 || map[pinky_loc[0]][pinky_loc[1] - 1] == 47)
    {
        moveup = 1;
    }
    else
    {
        moveup = 0;
    }
    if (map[pinky_loc[0]][pinky_loc[1] + 1] == 45 || map[pinky_loc[0]][pinky_loc[1] + 1] == 46 || map[pinky_loc[0]][pinky_loc[1] + 1] == 47)
    {
        movedown = 1;
    }
    else
    {
        movedown = 0;
    }


    // finds best direction
    int best_direction; // 1 = up, 2 = down, 3 = right, 4 = left
    // check if best direction is possible
    if (distance_y < -abs(distance_x) && moveup == 1)// pacman below blinky moves up
    {
        best_direction = 1;
    }
    else if (distance_y > abs(distance_x) && movedown == 1)
    {
        best_direction = 2;
    }
    else if (distance_x < -abs(distance_y) && moveleft == 1)
    {
        best_direction = 4;
    }
    else if (distance_x > abs(distance_y) && moveright == 1)
    {
        best_direction = 3;
    }
    // check if second best direction is possible
    else if (distance_y > 0 && distance_y < abs(distance_x) && movedown == 1)
    {
        best_direction = 2;
    }
    else if (distance_y < 0 && distance_y > -abs(distance_x) && moveup == 1)
    {
        best_direction = 1;
    }
    else if (distance_x > 0 && distance_x < abs(distance_y) && moveright == 1)
    {
        best_direction = 3;
    }
    else if (distance_x < 0 && distance_x > -abs(distance_y) && moveleft == 1)
    {
        best_direction = 4;
    }
    // check if third best direction is possible
    else if (abs(distance_y) < abs(distance_x) && (moveup == 1 && distance_y < 0))
    {
        best_direction = 1;
    }
    else if (abs(distance_y < abs(distance_x)) && (movedown == 1 && distance_y > 0))
    {
        best_direction = 2;
    }
    else if (abs(distance_y) > absf(distance_x) && (moveright == 1 && distance_x > 0))
    {
        best_direction = 3;
    }
    else if (abs(distance_y) > abs(distance_x) && (moveleft == 1 && distance_x < 0))
    {
        best_direction = 4;
    }
    // check if fourht best direction is possible
    else if (moveup == 1)
    {
        best_direction = 1;
    }
    else if (movedown == 1)
    {
        best_direction = 2;
    }
    else if (moveright == 1)
    {
        best_direction = 3;
    }
    else if (moveleft == 1)
    {
        best_direction = 4;
    }


    // updaye position based on best direction
    if (best_direction == 1)
    {
        pinky_loc[1]--;
    }
    else if (best_direction == 2)
    {
        pinky_loc[1]++;
    }
    else if (best_direction == 3)
    {
        pinky_loc[0]++;
    }
    else if (best_direction == 4)
    {
        pinky_loc[0]--;
    }
    return;
}

void blinky_update(float blinky_loc[2], float pacman_loc[2], float speed, int map[rows][columns], int pacman_mode, int ghost_mode)
{
    // to do 1. test scatter mode
    // 2. add scared mode
    // 3. move towards pacman
    // 4. prevent oscillation
    // 5. set to reset position if eaten
    

    // find distance from desired position
    float distance_x;
    float distance_y;
    if (pacman_mode == 2) // frightened mode
    {
        distance_y = pacman_loc[1] - blinky_loc[1];
        distance_x = pacman_loc[0] - blinky_loc[0];
    }
    else if (ghost_mode == 1) //  scatter mode
    {
        distance_y = blinky_loc[1] - 1;// change
        distance_x = blinky_loc[0] - rows + 1;
    }
    else // chase mode
    {
        distance_y = blinky_loc[1] - pacman_loc[1];
        distance_x = blinky_loc[0] - pacman_loc[0];
    }

    // find how far Blinky can move in each direction
    float moveupspeed;
    float movedownspeed;
    float moverightspeed;
    float moveleftspeed;
    if ((map[floor(blinky_loc[0] - .5)][ceil(blinky_loc[1] + speed + .5)] != 1 && 
    map[ceil(blinky_loc[0] + .5)][ceil(blinky_loc[1] + speed + .5)] != 1))
    {
        moverightspeed = speed;
    }
    else if (ceil(blinky_loc[1]) > blinky_loc[1])
    {
        moverightspeed = ceil(blinky_loc[1]) - blinky_loc[1];
    }
    else
    {
        moverightspeed = 0;
    }
    if ((map[floor(blinky_loc[0] - .5)][floor(blinky_loc[1] - speed - .5)] != 1 && 
    map[ceil(blinky_loc[0] + .5)][floor(blinky_loc[1] - speed - .5)] != 1))
    {
        moveleftspeed = speed;
    }
    else if (floor(blinky_loc[1]) < blinky_loc[1])
    {
        moveleftspeed = blinky_loc[1] - floor(blinky_loc[1]);
    }
    else
    {
        moveleftspeed = 0;
    }
    if ((map[floor(blinky_loc[0] - speed - .5)][floor(blinky_loc[1] - .5)] != 1 && 
    map[floor(blinky_loc[0] - speed - .5)][ceil(blinky_loc[1] + .5)] != 1))
    {
        moveupspeed = speed;
    }
    else if (floor(blinky_loc[0]) < blinky_loc[0])
    {
        moveupspeed = blinky_loc[1] - floor(blinky_loc[1]);
    }
    else
    {
        moveupspeed = 0;
    }
    if ((map[ceil(blinky_loc[0] + speed + .5)][ceil(blinky_loc[1] + .5)] != 1 && 
    map[ceil(blinky_loc[0] + speed + .5)][floor(blinky_loc[1] - .5)] != 1))
    {
        movedownspeed = speed;
    }
    else if (ceil(blinky_loc[0]) > blinky_loc[0])
    {
        movedownspeed = ceil(blinky_loc[0]) - blinky_loc[0];
    }
    else
    {
        movedownspeed = 0;
    }


    // finds best direction
    int best_direction; // 1 = up, 2 = down, 3 = right, 4 = left
    // check if the best direction is possible
    if (distance_y < -absf(distance_x) && moveupspeed != 0)// pacman below blinky moves up
    {
        best_direction = 1;
    }
    else if (distance_y > absf(distance_x) && movedownspeed != 0)
    {
        best_direction = 2;
    }
    else if (distance_x < -absf(distance_y) && moveleftspeed != 0)
    {
        best_direction = 4;
    }
    else if (distance_x > absf(distance_y) && moverightspeed != 0)
    {
        best_direction = 3;
    }
    // check if the second-best direction is possible
    else if (distance_y > 0 && distance_y < absf(distance_x) && moveupspeed != 0)
    {
        best_direction = 1;
    }
    else if (distance_y < 0 && distance_y > -absf(distance_x) && movedownspeed != 0)
    {
        best_direction = 2;
    }
    else if (distance_x > 0 && distance_x < absf(distance_y) && moveleftspeed != 0)
    {
        best_direction = 4;
    }
    else if (distance_x < 0 && distance_x > -absf(distance_y) && moverightspeed != 0)
    {
        best_direction = 3;
    }
    // check if the third-best direction is possible
    else if (absf(distance_y < absf(distance_x)) && (moveupspeed != 0 && distance_y < 0))
    {
        best_direction = 1;
    }
    else if (absf(distance_y < absf(distance_x)) && (movedownspeed != 0 && distance_y > 0))
    {
        best_direction = 2;
    }
    else if (absf(distance_y > absf(distance_x)) && (moverightspeed != 0 && distance_x > 0))
    {
        best_direction = 3;
    }
    else if (absf(distance_y > absf(distance_x)) && (moveleftspeed != 0 && distance_x < 0))
    {
        best_direction = 4;
    }
    // check if the fourth best direction is possible
    else if (moveupspeed != 0)
    {
        best_direction = 1;
    }
    else if (movedownspeed != 0)
    {
        best_direction = 2;
    }
    else if (moverightspeed != 0)
    {
        best_direction = 3;
    }
    else if (moveleftspeed != 0)
    {
        best_direction = 4;
    }


    //update position based on best direction
    if (best_direction == 1)
    {
        blinky_loc[1] -= moveupspeed;
    }
    else if (best_direction == 2)
    {
        blinky_loc[1] += movedownspeed;
    }
    else if (best_direction == 3)
    {
        blinky_loc[0] += moverightspeed;
    }
    else if (best_direction == 4)
    {
        blinky_loc[0] -= moveleftspeed;
    }
    return;
}

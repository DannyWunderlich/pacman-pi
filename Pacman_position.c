void Pacman_position(float pm_position [2], int movement, int map[rows][columns], float speed)
{
  // 1 = up, 2 = down, 3 = right, 4 = left
  // position represents center of pacman
  // 0 represents a wall
  // rows and columns will need to fit the map
  // speed < 1
  int current_row = pm_position[0];
  int  current_column = pm_position[1];
  if (movement == 1) // up
  {
    if (map[floor(pm_position[0] - speed - .5)][floor(pm_position[1] - .5)] == 0 || map[floor(pm_position[0] - speed - .5)][ceil(pm_position[1] + .5)] == 0)
    {
        pm_position[0] = ceil(pm_position[0] - .5) + .5;
    }
    else
    {
        pm_position[0] -= speed;
    }
  }
  else if (movement == 2) // down
  {
    if (map[ceil(pm_position[0] + speed + .5)][floor(pm_position[1] - .5)] == 0 || map[ceil(pm_position[0] + speed + .5)][ceil(pm_position[1] + .5)] == 0)
    {
        pm_position[0] = ceil(pm_position[0] + .5) - .5;
    }
    else
    {
        pm_position[0] += speed;
    }
  }
    else if (movement == 3) //right
  {
    if (map[ceil(pm_position[0] + .5)][ceil(pm_position[1] + speed + .5)] == 0 || map[floor(pm_position[0] - .5)][ceil(pm_position[1] + speed + .5)] == 0)
    {
        pm_position[1] = ceil(pm_position[1] + .5) - .5;
    }
    else
    {
        pm_position[1] += speed;
    }
  }
    else if (movement == 4) // left
  {
    if (map[ceil(pm_position[0] + .5)][floor(pm_position[1] - speed - .5)] == 0 || map[floor(pm_position[0] - .5)][floor(pm_position[1] - speed - .5)] == 0)
    {
        pm_position[1] = ceil(pm_position[1] - .5) + .5;
    }
    else
    {
        pm_position[1] -= speed;
    }
  }
  // add warp around
  return;
}

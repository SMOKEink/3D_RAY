#include "cub3d.h"

int	set_player_spawn(t_game *gm)
{
	int     y;
	int     x;
	char    c;
	const double fov = 0.66;

	y = 0;
	while (gm->map[y])
	{
		x = 0;
		while (gm->map[y][x])
		{
			c = gm->map[y][x];
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
			{
				gm->player.x = x + 0.5;
				gm->player.y = y + 0.5;
				if (c == 'N') { gm->player.dir_x = 0;
					gm->player.dir_y = -1; }
				if (c == 'S') { gm->player.dir_x = 0;
					gm->player.dir_y = 1; }
				if (c == 'E') { gm->player.dir_x = 1;
					gm->player.dir_y = 0; }
				if (c == 'W') { gm->player.dir_x = -1;
					gm->player.dir_y = 0; }
				gm->player.plane_x = gm->player.dir_y * fov;
				gm->player.plane_y = -gm->player.dir_x * fov;
				gm->map[y][x] = '0';
				return 1;
			}
			x++;
		}
		y++;
	}
	y = 0;
	while (gm->map[y])
	{
		x = 0;
		while (gm->map[y][x])
		{
			if (gm->map[y][x] == '0')
			{
				gm->player.x = x + 0.5;
				gm->player.y = y + 0.5;
				gm->player.dir_x = 1; // default east
				gm->player.dir_y = 0;
				gm->player.plane_x = gm->player.dir_y * fov;
				gm->player.plane_y = -gm->player.dir_x * fov;
				return 1;
			}
			x++;
		}
		y++;
	}
	//if somehow inside a wall, try to nudge to nearest floor
	if (is_wall(gm, (int)gm->player.x, (int)gm->player.y))
	{
		int dy = -1;
		while (dy <= 1)
		{
			int dx = -1;
			while (dx <= 1)
			{
				if (!is_wall(gm, (int)gm->player.x + dx, (int)gm->player.y + dy))
				{ gm->player.x += dx; gm->player.y += dy; dy = 2; break; }
				dx++;
			}
			dy++;
		}
	}
	return 0;
}
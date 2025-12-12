#include "cub3d.h"

static char map_get(t_game *gm, int mx, int my)
{
	if (my < 0 || my >= gm->map_h)
		return ('1');
	int rowlen = (int)strlen(gm->map[my]);
	if (mx < 0 || mx >= rowlen)
		return ('1');
	return (gm->map[my][mx]);
}

void draw_minimap(t_game *gm)
{
	//SETUP
	const int x_cen = 85;
	const int y_cen = 85;
	const int R  = 75;
	const int R2 = R * R;
	const double px_per_cell = 8.00;
	const double cell_per_px = 1.0 / px_per_cell;
	//FILL DISK AREA
	int	y = -R;
	while (++y <= R)
	{
		int py = y_cen + y;
		if (py < 0 || py >= HEIGHT)
			continue;
		int x = -R;
		while (++x <= R)
		{
			int px = x_cen + x;
			if (px < 0 || px >= WIDTH)
				continue;
			int d2 = x*x + y*y;
			if (d2 > R2)
				continue;
			put_pixel(gm, px, py, COL_ROAD);
			double wx = gm->player.x - x * cell_per_px;
            double wy = gm->player.y + y * cell_per_px;
			int mx = (int)floor(wx);
			int my = (int)floor(wy);
			char cub = map_get(gm, mx, my);
			if (cub == '1')
				put_pixel(gm, px, py, COL_WALL);
			else if (cub == 'D')
				put_pixel(gm, px, py, COL_DOOR);
			else if (cub == 'd')
				put_pixel(gm, px, py, COL_OPENED_DOOR);
		}
	}
	//PLAYER MARKER
	int oy = -2;
	while (++oy <= 2)
	{
		int ox = -2;
		while (++ox <= 2)
		{
			int px = x_cen + ox;
			int py = y_cen + oy;
			if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
				put_pixel(gm, px, py, COL_PLAYER);
		}
	}
	//DIRECTION RAY
	const double len_px = R * 0.3;
    double dxp = gm->player.dir_x;
    double dyp = gm->player.dir_y;
	double t = 0;
    while (t <= len_px)
    {
        int px = (int)(x_cen - dxp * t);
        int py = (int)(y_cen + dyp * t);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
			put_pixel(gm, px, py, COL_DIRECTION);
		t += 1.0;
    }
}
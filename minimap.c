#include "cub3d.h"

static inline unsigned int color(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

static char map_get(t_game *gm, int mx, int my)
{
	if (my < 0 || my >= gm->map_h)
		return ('1');
	int rowlen = (int)strlen(gm->map[my]);
	if (mx < 0 || mx >= rowlen)
		return ('1');
	return gm->map[my][mx];
}

void draw_minimap(t_game *gm)
{
	// Colors
	const unsigned int col_bg    = color(24, 24, 28);
	const unsigned int col_ring  = color(255, 255, 255);
	const unsigned int col_wall  = color(0, 0, 0);
	const unsigned int col_door  = color(100, 255, 100);
	const unsigned int col_door_open = color(40, 120, 40);
	const unsigned int col_player= color(255, 60, 60);
	const unsigned int col_fov   = color(255, 100, 100);
	const int cx = WIDTH - (MM_MARGIN_X + MM_RADIUS_PX);
	const int cy = HEIGHT - (MM_MARGIN_Y + MM_RADIUS_PX);
	const int R  = MM_RADIUS_PX;
	const int R2 = R * R;
	const double px_per_cell = (double)MM_PIX_PER_CELL;
	const double cell_per_px = 1.0 / px_per_cell;
	int	dy = -R;
	while (++dy <= R)
	{
		int py = cy + dy;
		if (py < 0 || py >= HEIGHT) continue;
		int dx = -R;
		while (++dx <= R)
		{
			int px = cx + dx;
			if (px < 0 || px >= WIDTH) continue;

			int d2 = dx*dx + dy*dy;
			if (d2 > R2)
				continue;
			put_pixel(gm, px, py, col_bg);
			double wx = gm->player.x - dx * cell_per_px;
            double wy = gm->player.y + dy * cell_per_px;

			int mx = (int)floor(wx);
			int my = (int)floor(wy);
			char cub = map_get(gm, mx, my);
			if (cub == '1')
				put_pixel(gm, px, py, col_wall);
			else if (cub == 'D')
				put_pixel(gm, px, py, col_door);
			else if (cub == 'd')
				put_pixel(gm, px, py, col_door_open);
		}
	}
	int a = -R;
	while (++a <= R)
	{
		int x1 = cx + a;
		int y1_top = cy - (int)sqrt((double)(R2 - a*a));
		int y1_bot = cy + (int)sqrt((double)(R2 - a*a));
		if (x1 >= 0 && x1 < WIDTH)
		{
			if (y1_top >= 0 && y1_top < HEIGHT) put_pixel(gm, x1, y1_top, col_ring);
			if (y1_bot >= 0 && y1_bot < HEIGHT) put_pixel(gm, x1, y1_bot, col_ring);
		}
	}
	int oy = -2;
	while (++oy <= 2)
	{
		int ox = -2;
		while (++ox <= 2)
		{
			int px = cx + ox;
			int py = cy + oy;
			if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
				put_pixel(gm, px, py, col_player);
		}
	}
	const double len_px = R * 0.3;
    double dxp = gm->player.dir_x;
    double dyp = gm->player.dir_y;
	double t = 0;
    while (t <= len_px)
    {
        int px = (int)(cx - dxp * t);
        int py = (int)(cy + dyp * t);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
			put_pixel(gm, px, py, col_fov);
		t += 1.0;
    }
}
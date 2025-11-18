#include "cub3d.h"

static inline unsigned int rgb(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

static char map_get(t_game *gm, int mx, int my)
{
	if (my < 0 || my >= gm->map_h) return '1'; // treat OOB as wall
	int rowlen = (int)strlen(gm->map[my]);
	if (mx < 0 || mx >= rowlen) return '1';
	return gm->map[my][mx];
}

void draw_minimap(t_game *gm)
{
	// Colors
	const unsigned int col_bg    = rgb(24, 24, 28);    // circle background
	const unsigned int col_ring  = rgb(255, 255, 255); // border ring
	const unsigned int col_wall  = rgb(0, 0, 0); // walls '1'
	const unsigned int col_door  = rgb(100, 255, 100);   // portals/doors 'D'
	const unsigned int col_player= rgb(255, 60, 60);   // player dot/arrow
	const unsigned int col_fov   = rgb(255, 100, 100);  // facing line

	// Circle center (bottom-left corner with margins)
	const int cx = WIDTH - (MM_MARGIN_X + MM_RADIUS_PX);
	const int cy = HEIGHT - (MM_MARGIN_Y + MM_RADIUS_PX);

	const int R  = MM_RADIUS_PX;
	const int R2 = R * R;

	// World-to-minimap scale
	const double px_per_cell = (double)MM_PIX_PER_CELL;
	const double cell_per_px = 1.0 / px_per_cell;

	// Draw filled circular background and content
	for (int dy = -R; dy <= R; ++dy)
	{
		int py = cy + dy;
		if (py < 0 || py >= HEIGHT) continue;
		for (int dx = -R; dx <= R; ++dx)
		{
			int px = cx + dx;
			if (px < 0 || px >= WIDTH) continue;

			int d2 = dx*dx + dy*dy;
			if (d2 > R2) continue; // outside circle

			// Background
			put_pixel(gm, px, py, col_bg);

			// Map sampling: convert pixel offset to world offset (in cells)
			double wx = gm->player.x - dx * cell_per_px;      // NEW: flip X
            double wy = gm->player.y + dy * cell_per_px;

			int mx = (int)floor(wx);
			int my = (int)floor(wy);
			char tile = map_get(gm, mx, my);

			// Draw nearest features only (inside circle). Prioritize solid features.
			if (tile == '1')
				put_pixel(gm, px, py, col_wall);
			else if (tile == 'D')
				put_pixel(gm, px, py, col_door);
		}
	}

	// Border ring
	for (int a = -R; a <= R; ++a)
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

	// Player dot at center
	for (int oy = -2; oy <= 2; ++oy)
	{
		for (int ox = -2; ox <= 2; ++ox)
		{
			int px = cx + ox;
			int py = cy + oy;
			if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
				put_pixel(gm, px, py, col_player);
		}
	}

	// Facing direction line
	const double len_px = R * 0.3;
    double dxp = gm->player.dir_x;
    double dyp = gm->player.dir_y;
    for (double t = 0; t <= len_px; t += 1.0)
    {
        // int px = (int)(cx + dxp * t); // OLD
        int px = (int)(cx - dxp * t);     // NEW: flip X direction
        int py = (int)(cy + dyp * t);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
            put_pixel(gm, px, py, col_fov);
    }
}
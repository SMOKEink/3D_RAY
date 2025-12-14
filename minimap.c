#include "cub3d.h"

static char	map_get(t_game *gm, int mx, int my)
{
	int	rowlen;

	if (my < 0 || my >= gm->map_h)
		return ('1');
	rowlen = (int)strlen(gm->map[my]);
	if (mx < 0 || mx >= rowlen)
		return ('1');
	return (gm->map[my][mx]);
}

static void	draw_player_marker(t_game *gm)
{
	int y;
	int x;
	int px;
	int py;

	y = -2;
	while (y <= 2)
	{
		x = -2;
		while (x <= 2)
		{
			px = MMAP_X + x;
			py = MMAP_Y + y;
			if (px >= 0 && py >= 0 && px < WIDTH && py < HEIGHT)
				put_pixel(gm, px, py, COL_PLAYER);
			x++;
		}
		y++;
	}
}

static void	draw_player_direction(t_game *gm)
{
	double	step;
	double	dir_len;
	double	dir_x;
	double	dir_y;
	int		px;
	int		py;

	dir_len = 0.3 * MMAP_R;
	dir_x = gm->player.dir_x;
	dir_y = gm->player.dir_y;
	step = 0;
	while (step <= dir_len)
	{
		px = (int)(MMAP_X - dir_x * step);
		py = (int)(MMAP_Y + dir_y * step);
		if (px >= 0 && py >= 0 && px < WIDTH && py < HEIGHT)
			put_pixel(gm, px, py, COL_DIRECTION);
		step++;
	}
}
// static int	pixel_in_bounds(int px, int py)
// {
// 	if (px < 0 || px >= WIDTH)
// 		return (1);
// 	if (py < 0 || py >= HEIGHT)
// 		return (1);
// 	return (0);
// }
static void	draw_disk_pixel(t_game *gm, int px, int py)
{
	int		x_off;
	int		y_off;
	double	wx;
	double	wy;
	double	cell_per_px;
	char	cell;

	cell_per_px = 1.0 / MMAP_PIX_PER_CELL;
	x_off = px - MMAP_X;
	y_off = py - MMAP_Y;
	wx = gm->player.x - x_off * cell_per_px;
	wy = gm->player.y + y_off * cell_per_px;
	cell = map_get(gm, (int)floor(wx), (int)floor(wy));
	if (cell == '1')
		put_pixel(gm, px, py, COL_WALL);
	else if (cell == 'D')
		put_pixel(gm, px, py, COL_DOOR);
	else if (cell == 'd')
		put_pixel(gm, px, py, COL_OPENED_DOOR);
}
static void	draw_minimap_disk(t_game *gm)
{
	int		x;
	int		y;
	int		px;
	int		py;
	int		r_sq;

	r_sq = MMAP_R * MMAP_R;
	y = -MMAP_R - 1;
	while (++y <= MMAP_R)
	{
		py = MMAP_Y + y;
		if (py < 0 || py >= HEIGHT)
			continue;
		x = -MMAP_R - 1;
		while (++x <= MMAP_R)
		{
			if (x * x + y * y > r_sq)
				continue;
			px = MMAP_X + x;
			if (px < 0 || px >= WIDTH)
				continue;
			put_pixel(gm, px, py, COL_ROAD);
			draw_disk_pixel(gm, px, py);
		}
	}
}
void	draw_minimap(t_game *gm)
{
	draw_minimap_disk(gm);
	draw_player_marker(gm);
	draw_player_direction(gm);
}
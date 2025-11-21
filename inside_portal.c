#include "cub3d.h"

bool    inside_portal(t_game *gm, int map_x, int map_y, double side_distx, double side_disty, double ray_dir_x, double ray_dir_y, int x, int y)
{
    bool inside_portal = false;
	if (map_y >= 0 && map_y < gm->map_h)
	{
		int rowlen = (int)strlen(gm->map[map_y]);
		if (map_x >= 0 && map_x < rowlen && gm->map[map_y][map_x] == 'D')
			inside_portal = true;
	}
	if (inside_portal)
	{
		bool vert_side = (side_distx < side_disty);
		double portal_dist;
		if (vert_side == true)
			portal_dist = side_distx;
		else
			portal_dist = side_disty;
		double perp_dist = portal_dist;
		int line_len = (int)(HEIGHT / perp_dist);
		int draw_start = -line_len / 2 + HEIGHT / 2;
		int draw_end = line_len / 2 + HEIGHT / 2;
		if (draw_start < 0) draw_start = 0;
		if (draw_end >= HEIGHT) draw_end = HEIGHT - 1;
		double wall_x;
		if (vert_side)
			wall_x = gm->player.y + perp_dist * ray_dir_y;
		else
			wall_x = gm->player.x + perp_dist * ray_dir_x;
		wall_x -= floor(wall_x);
		int tex_x = (int)(wall_x * (double)gm->door.w);
		if (vert_side && ray_dir_x > 0)
			tex_x = gm->door.w - tex_x - 1;
		if (!vert_side && ray_dir_y < 0)
			tex_x = gm->door.w - tex_x - 1;
		double step = (double)gm->door.h / (double)line_len;
		double tex_pos = (draw_start - HEIGHT / 2.0 + line_len / 2.0) * step;
		y = draw_start - 1;
		while (++y <= draw_end)
		{
			int tex_y = (int)tex_pos;
			tex_pos += step;
			put_pixel(gm, x, y, texel_at(&gm->door, tex_x, tex_y));
		}
        return (1);
	}
	return (0);
}
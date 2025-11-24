#include "cub3d.h"

unsigned int texel_at(t_tex *t, int x, int y)
{
	char *px;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= t->w)
		x = t->w - 1;
	if (y >= t->h)
		y = t->h - 1;
	px = t->data + y * t->line_len + x * (t->bpp / 8);
	return *(unsigned int *)px;
}

void	render_image(t_game *gm)
{
	int		x;
	int		y;

	y = -1;
	while (++y <= HEIGHT / 2)
	{
		x = -1;
		while (++x < WIDTH)
			put_pixel(gm, x, y, gm->ceiling);
	}
	y = HEIGHT / 2;
	while (++y < HEIGHT)
	{
		x = -1;
		while (++x < WIDTH)
			put_pixel(gm, x, y, gm->floor);
	}

	x = -1;
	while (++x <= WIDTH)
	{
	//1// compute ray direction
		double	cam_x = 2 * x / (double)WIDTH - 1;
		// double cam_x = 1;
		double	ray_dir_x = gm->player.dir_x + gm->player.plane_x * cam_x;
		double	ray_dir_y = gm->player.dir_y + gm->player.plane_y * cam_x;
	//2// initialize DDA
		double side_distx;
		double side_disty;
		int	x_step;
		int	y_step;
		double t_x;
		double t_y;
		int map_x = (int)gm->player.x;
		int map_y = (int)gm->player.y;
		if (ray_dir_x != 0)
			t_x = 1 / fabs(ray_dir_x);
		else
			t_x = INFINITY;
		if (ray_dir_y != 0)
			t_y = 1 / fabs(ray_dir_y);
		else
			t_y = INFINITY;
		if (ray_dir_x < 0)
		{
			x_step = -1;
			side_distx = (gm->player.x - map_x) * t_x; // Δx * t_x
		}
		else
		{
			x_step = 1;
			side_distx = (map_x + 1.0 - gm->player.x) * t_x;
		}
		if (ray_dir_y < 0)
		{
			y_step = -1;
			side_disty = (gm->player.y - map_y) * t_y;	// Δx * t_x
		}
		else
		{
			y_step = 1;
			side_disty = (map_y + 1.0 - gm->player.y) * t_y;
		}
	// IF INSIDE PORTAL
		// if (inside_portal(gm, map_x, map_y, side_distx, side_disty, ray_dir_x, ray_dir_y, x, y))
		// 	continue;
	//3// Perform DDA
		bool	hit = false;
		bool	v_side = false;
		while (!hit)
		{
			if (side_distx < side_disty)
			{
				map_x += x_step;
				side_distx += t_x;
				v_side = true;
			}
			else
			{
				map_y += y_step;
				side_disty += t_y;
				v_side = false;
			}
			if (is_solid_for_ray(gm, map_x, map_y))
				hit = true;
		}
	//4// claculate the perpendicular wall distance
		double	perp_dist;
		// perp_dist = (map_x - gm->player.x + (1 - x_step) / 2) / ray_dir_x; // equivalent to : if (x_step == 1) ==> Δx / ray_dir_x       else if (x_step == -1) ==> (Δx + 1) / ray_dir_x
		if (v_side == true)
			perp_dist = side_distx - t_x;
		else
			perp_dist = side_disty - t_y;
	//5// calculate wall height
		int	line_len = (int)(HEIGHT / perp_dist);
		
	//6// draw the wall column
		t_tex	*tex;
		char	cub = '1';
		if (map_y >= 0 && map_y < gm->map_h)
		{
			int rowlen = (int)strlen(gm->map[map_y]);
			if (map_x >= 0 && map_x < rowlen)
				cub = gm->map[map_y][map_x];
		}
		if (cub == 'D')
			tex = &gm->door;
		else if (cub == 'd')
			continue ;
		else
		{
			if (v_side)
			{
				if (ray_dir_x > 0)
					tex =  &gm->wall[TEX_W];
				else
					tex = &gm->wall[TEX_E];
			}
			else
			{
				if (ray_dir_y > 0)
					tex = &gm->wall[TEX_N];
				else
					tex = &gm->wall[TEX_S];
			}
		}
		double wall_x;	// compute the exact position where the ray heats the wall
		if (v_side)		//between 0 and 1
			wall_x = gm->player.y + perp_dist * ray_dir_y;
		else
			wall_x = gm->player.x + perp_dist * ray_dir_x;
		wall_x -= (int)wall_x;

		int tex_x = (int)(wall_x * (double)tex->w);
		if (v_side && ray_dir_x > 0)
			tex_x = tex->w - tex_x - 1;
		if (!v_side && ray_dir_y < 0)
			tex_x = tex->w - tex_x - 1;

		int	line_start = HEIGHT / 2 - line_len / 2;
		int	line_end = HEIGHT / 2 + line_len / 2;
		if (line_start < 0)
			line_start = 0;
		if (line_end >= HEIGHT)
			line_end = HEIGHT - 1;
		double tex_step = (double)tex->h / (double)line_len;
		double tex_pos = (line_start - (HEIGHT / 2.0 - line_len / 2.0)) * tex_step;
											//ideal to
		// printf("line_start = %d.00\nideal_top = %.2lf\ntex_pos = %.2lf\n\n", line_start, (HEIGHT / 2.0 - line_len / 2.0), tex_pos);
	//7// draw textured wall column
		y = line_start - 1;
		while (++y <= line_end)
		{
			// int tex_y = (int)tex_pos;
			tex_pos += tex_step;
			put_pixel(gm, x, y, texel_at(tex, tex_x, (int)tex_pos));
		}
	}
}
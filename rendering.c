#include "cub3d.h"

unsigned int    texel_at(t_tex *t, int x, int y)
{
	char    *px;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= t->w)
		x = t->w - 1;
	if (y >= t->h)
		y = t->h - 1;
	px = t->data + y * t->line_len + x * (t->bpp / 8);
	return (*(unsigned int *)px);
}

void    ceil_and_floor(t_game *gm)
{
	int x;
	int y;

	y = 0;
	while (y <= HEIGHT / 2)
	{
		x = 0;
		while (x < WIDTH)
		{
			put_pixel(gm, x, y, gm->ceiling);
			x++;
		}
		y++;
	}
	y = HEIGHT / 2;
	while (y < HEIGHT)
	{
		x = 0;
		while (x < WIDTH)
		{
			put_pixel(gm, x, y, gm->floor);
			x++;
		}
		y++;
	}
}

static void init_ray(t_game *gm, int x, t_ray *ray)
{
	ray->cam_x = 2 * x / (double)WIDTH - 1;
	ray->dir_x = gm->player.dir_x + gm->player.plane_x * ray->cam_x;
	ray->dir_y = gm->player.dir_y + gm->player.plane_y * ray->cam_x;
	ray->map_x = (int)gm->player.x;
	ray->map_y = (int)gm->player.y;
	if (ray->dir_x != 0)
		ray->delta_x = fabs(1 / ray->dir_x);
	else
		ray->delta_x = INFINITY;
	if (ray->dir_y != 0)
		ray->delta_y = fabs(1 / ray->dir_y);
	else
		ray->delta_y = INFINITY;
}

static void init_first_steps(t_game *gm, t_ray *ray)
{
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_x = (gm->player.x - ray->map_x) * ray->delta_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_x = (ray->map_x + 1 - gm->player.x) * ray->delta_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_y = (gm->player.y - ray->map_y) * ray->delta_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_y = (ray->map_y + 1 - gm->player.y) * ray->delta_y;
	}
}

static void dda_loop(t_game *gm, t_ray *ray)
{
	bool    hit;

	hit = false;
	while (!hit)
	{
		if (ray->side_x < ray->side_y)
		{
			ray->map_x += ray->step_x;
			ray->side_x += ray->delta_x;
			ray->hit_vertical = true;
		}
		else
		{
			ray->map_y += ray->step_y;
			ray->side_y += ray->delta_y;
			ray->hit_vertical = false;
		}
		if (is_wall(gm, ray->map_x, ray->map_y))
			hit = true;
	}
	if (ray->hit_vertical)
		ray->perp_dist = ray->side_x - ray->delta_x;
	else
		ray->perp_dist = ray->side_y - ray->delta_y;
}


static char map_cell(t_game *gm, int x, int y)
{
	int	len;

	if (y < 0 || y >= gm->map_h)
		return ('1');
	len = strlen(gm->map[y]);
	if (x < 0 || x >= len)
		return ('1');
	return (gm->map[y][x]);
}

static t_tex *choose_texture(t_game *gm, t_ray *ray)
{
	char    cell;

	cell = map_cell(gm, ray->map_x, ray->map_y);
	if (cell == 'D')
		return (&gm->door);
	if (cell == 'd')
		return (NULL);
	if (ray->hit_vertical)
	{
		if (ray->dir_x > 0)
			return (&gm->wall[TEX_W]);
		return (&gm->wall[TEX_E]);
	}
	if (ray->dir_y > 0)
		return (&gm->wall[TEX_N]);
	else
		return (&gm->wall[TEX_S]);
}

static int hit_position(t_game *gm, t_tex *tex, t_ray *ray)
{
	int		tex_x;
	double	hit_point;

	if (ray->hit_vertical)
		hit_point = gm->player.y + ray->perp_dist * ray->dir_y;
	else
		hit_point = gm->player.x + ray->perp_dist * ray->dir_x;
	hit_point -= floor(hit_point);
	tex_x = (int)(hit_point * (double)tex->w);
	if (ray->hit_vertical && ray->dir_x > 0) //correct mirror textures
		tex_x = tex->w - tex_x;
	if (!ray->hit_vertical && ray->dir_y < 0)
		tex_x = tex->w - tex_x;
	return (tex_x);
}

static void draw_slice(t_game *gm, t_slice *slice, t_tex *tex, t_ray *ray)
{
	int             y;
	unsigned int    color;
	double			real_start;

	slice->line_len = (int)(HEIGHT / ray->perp_dist);
	if (slice->line_len <= 0)
		slice->line_len = 1;
	slice->line_start = HEIGHT / 2 - slice->line_len / 2;
	slice->line_end = HEIGHT / 2 + slice->line_len / 2;
	if (slice->line_start < 0)
		slice->line_start = 0;
	if (slice->line_end >= HEIGHT)
		slice->line_end = HEIGHT - 1;
	slice->tex_step = (double)tex->h / (double)slice->line_len;
	real_start = HEIGHT / 2.0 - slice->line_len / 2.0;
	slice->tex_pos = (slice->line_start - real_start) * slice->tex_step;
	y = slice->line_start - 1;
	while (++y <= slice->line_end)
	{
		slice->tex_y = (int)slice->tex_pos;
		color = texel_at(tex, slice->tex_x, slice->tex_y);
		put_pixel(gm, slice->column, y, color);
		slice->tex_pos += slice->tex_step;
	}
}

void    render_image(t_game *gm)
{
	int     x;
	t_ray   ray;
	t_tex   *tex;
	t_slice slice;

	ceil_and_floor(gm);
	x = 0;
	while (x < WIDTH)
	{
		init_ray(gm, x, &ray);
		init_first_steps(gm, &ray);
		dda_loop(gm, &ray);
		tex = choose_texture(gm, &ray);
		if (tex)
		{
			slice.column = x;
			slice.tex_x = hit_position(gm, tex, &ray);
			draw_slice(gm, &slice, tex, &ray);
		}
		x++;
	}
}


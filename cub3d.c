#include "cub3d.h"

void destroy_game(t_game *g, const char *msg)
{
	if (msg)
		fprintf(stderr, "%s\n", msg);
	if (g->mlx && g->win)
		mlx_mouse_show(g->mlx, g->win);
	free_textures(g);
	if (g->frame.img && g->mlx)
		mlx_destroy_image(g->mlx, g->frame.img);
	if (g->win && g->mlx)
		mlx_destroy_window(g->mlx, g->win);
	if (g->mlx)
	{
		mlx_destroy_display(g->mlx);
		free(g->mlx);
	}
	free_map(g->map);
	exit(0);
}
bool	is_wall(t_game *gm, int x, int y)
{
	if (x < 0 || y < 0 || y >= gm->map_h)
		return (1);
	int row_width = (int)strlen(gm->map[y]);
	if (x >= row_width)
		return (1);
	char c = gm->map[y][x];
	if (c == '1' || c == ' ')
		return (1);
	return (0);
}


bool	is_solid_for_ray(t_game *gm, int x, int y)
{
    if (x < 0 || y < 0 || y >= gm->map_h)
        return (1);
    int row_width = (int)strlen(gm->map[y]);
    if (x >= row_width)
        return (1);
    char c = gm->map[y][x];
    if (c == '1' || c == ' ' || c == 'D')
        return (1);
    return (0);
}

void	clear_image(t_game *game)
{
	int	i;
	int	total;
	int	*img_data;

	img_data = (int *)game->frame.data_img;
	total = (game->frame.line_len / 4) * HEIGHT;
	i = -1;
	while (++i < total)
		img_data[i] = 0;
}

void	put_pixel(t_game *gm, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return ;
	dst = gm->frame.data_img + y * gm->frame.line_len + x * (gm->frame.bpp / 8);
	*(unsigned int *)dst = color;
}

static int	set_player_spawn(t_game *gm)
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
				if (c == 'N') { gm->player.dir_x = 0;  gm->player.dir_y = -1; }
				if (c == 'S') { gm->player.dir_x = 0;  gm->player.dir_y = 1; }
				if (c == 'E') { gm->player.dir_x = 1;  gm->player.dir_y = 0; }
				if (c == 'W') { gm->player.dir_x = -1; gm->player.dir_y = 0; }
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
	return 0;
}

static int load_tex_any(t_game *gm, t_tex *tex, const char *path)
{
	tex->img = mlx_xpm_file_to_image(gm->mlx, (char *)path, &tex->w, &tex->h);
	if (!tex->img)
	{
		fprintf(stderr, "Texture load failed: %s\n", path);
		return 1;
	}
	tex->data = mlx_get_data_addr(tex->img, &tex->bpp, &tex->line_len, &tex->endian);
	if (!tex->data)
	{
		fprintf(stderr, "Data addr failed: %s\n", path);
		return 1;
	}
	return 0;
}
static int load_tex(t_game *gm, int idx, const char *path)
{
	t_tex *t = &gm->wall[idx];

	t->img = mlx_xpm_file_to_image(gm->mlx, (char *)path, &t->w, &t->h);
	if (!t->img)
	{
		fprintf(stderr, "Texture load failed: %s\n", path);
		return 1;
	}
	t->data = mlx_get_data_addr(t->img, &t->bpp, &t->line_len, &t->endian);
	if (!t->data)
	{
		fprintf(stderr, "Data addr failed: %s\n", path);
		return 1;
	}
	return 0;
}

int load_textures(t_game *gm)
{
	int i = 0;
	while (i < TEX_WALL_COUNT)
	{
		if (load_tex(gm, i, gm->tex_path[i]))
			return 1;
		i++;
	}
	if (gm->door_path && load_tex_any(gm, &gm->door, gm->door_path))
        return 1;
	return 0;
}

void free_textures(t_game *gm)
{
	int i = 0;
	while (i < TEX_WALL_COUNT)
	{
		if (gm->wall[i].img && gm->mlx)
			mlx_destroy_image(gm->mlx, gm->wall[i].img);
		gm->wall[i].img = NULL;
		gm->wall[i].data = NULL;
		i++;
	}

	i = 0;
	while (i < TEX_WALL_COUNT)
	{
		if (gm->tex_path[i])
			free(gm->tex_path[i]);
		gm->tex_path[i] = NULL;
		i++;
	}
	if (gm->door_path)
	{
		if (gm->door.img && gm->mlx)
        mlx_destroy_image(gm->mlx, gm->door.img);
	    gm->door.img = NULL;
	    gm->door.data = NULL;
		free(gm->door_path);
		gm->door_path = NULL;
	}
	// free_hand_textures(gm);
}

int	init_game(t_game *gm, char *filename)
{
	memset(gm, 0, sizeof(*gm));
	gm->mlx = mlx_init();
	if (!gm->mlx)
		return (1);
	gm->win = mlx_new_window(gm->mlx, WIDTH, HEIGHT, "CUB3D");
	if (!gm->win)
		return (1);
	gm->frame.img = mlx_new_image(gm->mlx, WIDTH, HEIGHT);
	if (!gm->frame.img)
		return (1);
	gm->frame.data_img = mlx_get_data_addr(gm->frame.img, &gm->frame.bpp, &gm->frame.line_len, &gm->frame.endian);
	if (!gm->frame.data_img)
		return (1);
	if (parse_config(gm, filename))
		return (1);
	if (load_textures(gm))
	{
		destroy_game(gm, "Failed to load textures\n");
		return 1;
	}
	// compute max width for reference (not used by is_wall)
	gm->map_w = (int)strlen(gm->map[0]);
	{
		int i = 1;
		while (gm->map[i])
		{
			int len = (int)strlen(gm->map[i]);
			if (len > gm->map_w)
				gm->map_w = len;
			i++;
		}
	}
	if (!set_player_spawn(gm))
		return (1);
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
	mlx_mouse_hide(gm->mlx, gm->win);
	return (0);
}

int close_win(t_game *g)
{
	destroy_game(g, NULL);
	return (0);
}

int	main_function(t_game *gm)
{
	move_player(gm);
	render_image(gm);
	draw_minimap(gm);
	mlx_put_image_to_window(gm->mlx, gm->win, gm->frame.img, 0, 0);
	return (0);
}

int main(int ac, char **av)
{
	t_game	gm;

	if (ac != 2 || !av[1] || !av[1][0])
		return (printf("Error\n"), 1);
	if (init_game(&gm, av[1]))
		return (1);
	mlx_loop_hook(gm.mlx, main_function, &gm);
	mlx_hook(gm.win, 2, KeyPressMask, key_press, &gm);
	mlx_hook(gm.win, 3, KeyReleaseMask, key_release, &gm);
	mlx_hook(gm.win, 17, 0, close_win, &gm);
	mlx_hook(gm.win, MotionNotify, PointerMotionMask, mouse_move, &gm);
	mlx_loop(gm.mlx);
	return (0);
}
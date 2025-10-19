#include "cub3d.h"

static char *g_map[] = {
    "111111111111111111111111",
    "100000000000000000000001",
    "101111011111111111101101",
    "101000010000000001001001",
    "101011110111111101111101",
    "101010000100000101000001",
    "101011110111S11011101101",
    "101000010000000000010001",
    "111110111111011111011101",
    "100010100001010000010001",
    "101110111101011111011101",
    "101000000001000000000101",
    "101011111101111111110101",
    "101000100000100000010001",
    "101110111111111111011101",
    "1000000000000000000000N1",
    "111111111111111111111111",
    NULL
};

static void fill_checker_texture(t_game *gm, t_tex *t, int c1, int c2)
{
	int w = 64, h = 64;
	t->w = w; t->h = h;
	t->img.img = mlx_new_image(gm->mlx, w, h);
	t->img.data_img = mlx_get_data_addr(t->img.img, &t->img.bpp, &t->img.line_len, &t->img.endian);
	int tile = 8;
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int use_c1 = ((x / tile) + (y / tile)) & 1;
			char *dst = t->img.data_img + y * t->img.line_len + x * (t->img.bpp / 8);
			*(unsigned int *)dst = use_c1 ? c1 : c2;
		}
	}
}

int	init_game(t_game *gm)
{
	double	fov;

	memset(gm, 0, sizeof(*gm));
	gm->mlx = mlx_init();
	if (!gm->mlx)
		return (1);
	gm->win = mlx_new_window(gm->mlx, 800, 600, "cub3d");
	if (!gm->win)
		return (1);
	gm->frame.img = mlx_new_image(gm->mlx, 800, 600);
	if (!gm->frame.img)
		return (1);
	gm->frame.data_img = mlx_get_data_addr(gm->frame.img, &gm->frame.bpp, &gm->frame.line_len, &gm->frame.endian);
	if (!gm->frame.data_img)
		return (1);
	gm->height = 600;
	gm->width = 800;
	gm->map = g_map;
	gm->map_h = 0;
    while (gm->map[gm->map_h])
        gm->map_h++;
    gm->map_w = (int)strlen(gm->map[0]);
	gm->player.x = 5.5;
	gm->player.y = 5.5;
	gm->player.dir_x = -1;
	gm->player.dir_y = 0;
	fov = 0.66;
	gm->player.plane_x = gm->player.dir_y * fov;
	gm->player.plane_y = -gm->player.dir_x * fov;
	mlx_mouse_hide(gm->mlx, gm->win);

	// Load 4 wall textures (N,S,W,E)
	const char *paths[4] = {
		"assets/north.xpm",
		"assets/south.xpm",
		"assets/west.xpm",
		"assets/east.xpm"
	};
	for (int i = 0; i < 4; ++i)
	{
		gm->tex[i].img.img = mlx_xpm_file_to_image(gm->mlx, (char *)paths[i], &gm->tex[i].w, &gm->tex[i].h);
		if (!gm->tex[i].img.img)
		{
			// Fallback: generate procedural checker if file missing
			int cA[4] = {0x6A6AFF, 0xFF6A6A, 0x6AFF6A, 0xFFFF6A};
			int cB[4] = {0x2A2A80, 0x802A2A, 0x2A802A, 0x80802A};
			fprintf(stderr, "Failed to load %s, generating checker fallback.\n", paths[i]);
			fill_checker_texture(gm, &gm->tex[i], cA[i], cB[i]);
		}
		else
		{
			gm->tex[i].img.data_img = mlx_get_data_addr(gm->tex[i].img.img, &gm->tex[i].img.bpp, &gm->tex[i].img.line_len, &gm->tex[i].img.endian);
			if (!gm->tex[i].img.data_img)
				return (1);
		}
	}
	return (0);
}

bool	is_wall(t_game *gm, int x, int y)
{
	if (x < 0 || y < 0 || x >= gm->map_w || y >= gm->map_h)
		return (1);
	if (gm->map[y][x] == '1')
		return (1);
	else
		return (0);
}

void	move_player(t_game *gm)
{
	t_player *p = &gm->player;
	double	nx, ny;
	double	angle;
	double prev_dir_x, prev_plane_x;

	if (gm->keys.up)
	{
		nx = p->x + p->dir_x * 0.01;
		ny = p->y + p->dir_y * 0.01;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.down)
	{
		nx = p->x - p->dir_x * 0.01;
		ny = p->y - p->dir_y * 0.01;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.right) // (y, -x)
	{
		nx = p->x + p->dir_y * 0.01;
		ny = p->y - p->dir_x * 0.01;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.left) // (-y, x)
	{
		nx = p->x - p->dir_y * 0.01;
		ny = p->y + p->dir_x * 0.01;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.rot_r || gm->keys.rot_l)
	{
		// x' = x . cos(p) - y . sin(p)
		// y' = x . sin(p) + y . cos(p)
		if (gm->keys.rot_r)
			angle = 0.045;
		else
			angle = -0.045;
		prev_dir_x = p->dir_x;
		p->dir_x = p->dir_x * cos(angle) - p->dir_y * sin(angle);
		p->dir_y = prev_dir_x * sin(angle) + p->dir_y * cos(angle);
		prev_plane_x = p->plane_x;
		p->plane_x = p->plane_x * cos(angle) - p->plane_y * sin(angle);
		p->plane_y = prev_plane_x * sin(angle) + p->plane_y * cos(angle);
	}	
}

void	clear_image(t_game *game)
{
	int	i;
	int	total;
	int	*img_data;

	img_data = (int *)game->frame.data_img;
	total = (game->frame.line_len / 4) * game->height;
	i = -1;
	while (++i < total)
		img_data[i] = 0x202020;
}

void	put_pixel(t_game *gm, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= gm->width || y >= gm->height)
		return ;
	dst = gm->frame.data_img + y * gm->frame.line_len + x * (gm->frame.bpp / 8);
	*(unsigned int *)dst = color;
}

void	render_image(t_game *gm)
{
	int		x;
	int		y;

	clear_image(gm);
	y = -1;
	while (++y < gm->height / 2)
	{
		x = -1;
		while (++x < gm->width)
			put_pixel(gm, x, y, 0x87CEFA);
	}
	y = gm->height / 2;
	while (++y < gm->height)
	{
		x = -1;
		while (++x < gm->width)
			put_pixel(gm, x, y, 0xD2B48C);
	}

	x = -1;
	while (++x < gm->width)
	{
	//1// compute ray direction
		double	cam_x = 2 * x / (double)gm->width - 1;
		double	ray_dir_x = gm->player.dir_x + gm->player.plane_x * cam_x;
		double	ray_dir_y = gm->player.dir_y + gm->player.plane_y * cam_x;
	//2// initialize DDA
		int map_x = (int)gm->player.x;
		int map_y = (int)gm->player.y;
		double delta_dist_x;
		double delta_dist_y;
		if (ray_dir_x != 0)
			delta_dist_x = fabs(1 / ray_dir_x);
		else
			delta_dist_x = 1e30;
		if (ray_dir_y != 0)
			delta_dist_y = fabs(1 / ray_dir_y);
		else
			delta_dist_y = 1e30;
		double side_dist_x;
		double side_dist_y;
		int	step_x;
		int	step_y;
		bool	hit = 0;
		bool	side = 0;
		if (ray_dir_x < 0)
		{
			step_x = -1;
			side_dist_x = (gm->player.x - map_x) * delta_dist_x;
		}
		else
		{
			step_x = 1;
			side_dist_x = (map_x + 1.0 - gm->player.x) * delta_dist_x;
		}
		if (ray_dir_y < 0)
		{
			step_y = -1;
			side_dist_y = (gm->player.y - map_y) * delta_dist_y;
		}
		else
		{
			step_y = 1;
			side_dist_y = (map_y + 1.0 - gm->player.y) * delta_dist_y;
		}
	//3// Perform DDA
		while (!hit)
		{
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = 1;
			}
			if (is_wall(gm, map_x, map_y))
				hit = true;
		}
	//4// claculate the perpendicular wall distance
		double	per_wall_dist;
		if (side == 0)
			per_wall_dist = (map_x - gm->player.x + (1 - step_x) / 2) / ray_dir_x;
		else
			per_wall_dist = (map_y - gm->player.y + (1 - step_y) / 2) / ray_dir_y;
		//5// calculate wall height
		int	line_height = (int)(gm->height / per_wall_dist);
		int	draw_start = -line_height / 2 + gm->height / 2;
		int	draw_end = line_height / 2 + gm->height / 2;
		if (draw_start < 0)
			draw_start = 0;
		if (draw_end >= gm->height)
			draw_end = gm->height - 1;
		//6// choose texture index by side and ray direction
		int tex_id;
		// side==0: hit vertical wall (x-side). Ray to the right hits West wall; to the left hits East wall.
		// side==1: hit horizontal wall (y-side). Ray up hits North wall; down hits South wall.
		if (side == 0)
			tex_id = (ray_dir_x > 0) ? 2 /* West */ : 3 /* East */;
		else
			tex_id = (ray_dir_y > 0) ? 0 /* North */ : 1 /* South */;
		t_tex *tex = &gm->tex[tex_id];

		// Find exact hit position on wall to compute texture X coordinate
		double wall_x;
		if (side == 0)
			wall_x = gm->player.y + per_wall_dist * ray_dir_y;
		else
			wall_x = gm->player.x + per_wall_dist * ray_dir_x;
		wall_x -= floor(wall_x);
		int tex_x = (int)(wall_x * (double)tex->w);
		// Flip texture horizontally for certain sides for consistency
		if (side == 0 && ray_dir_x > 0) tex_x = tex->w - tex_x - 1;
		if (side == 1 && ray_dir_y < 0) tex_x = tex->w - tex_x - 1;

		// Step per screen pixel in texture space
		double step = (double)tex->h / (double)line_height;
		double tex_pos = (draw_start - gm->height / 2 + line_height / 2) * step;
		y = draw_start - 1;
		while (++y <= draw_end)
		{
			int tex_y = (int)tex_pos & (tex->h - 1);
			tex_pos += step;
			// Sample pixel from texture
			char *src = tex->img.data_img + tex_y * tex->img.line_len + tex_x * (tex->img.bpp / 8);
			unsigned int color = *(unsigned int *)src;
			put_pixel(gm, x, y, color);
		}
	}
}

int	main_loop(t_game *gm)
{
	move_player(gm);
	render_image(gm);
	mlx_put_image_to_window(gm->mlx, gm->win, gm->frame.img, 0, 0);
	return (0);
}
void destroy_game(t_game *g, const char *msg)
{
	if (msg)
		fprintf(stderr, "%s\n", msg);
	if (g->mlx && g->win)
		mlx_mouse_show(g->mlx, g->win);
	if (g->frame.img && g->mlx)
		mlx_destroy_image(g->mlx, g->frame.img);
	if (g->win && g->mlx)
		mlx_destroy_window(g->mlx, g->win);
	if (g->mlx)
	{
		mlx_destroy_display(g->mlx);
		free(g->mlx);
	}
	exit(0);
}
int close_win(t_game *g)
{
	destroy_game(g, NULL);
	return 0;
}
int key_press(int keycode, t_game *g)
{
	if (keycode == XK_Escape) close_win(g);
	else if (keycode == XK_w) g->keys.up = 1;
	else if (keycode == XK_s) g->keys.down = 1;
	else if (keycode == XK_a) g->keys.left = 1;
	else if (keycode == XK_d) g->keys.right = 1;
	else if (keycode == XK_Left) g->keys.rot_l = 1;
	else if (keycode == XK_Right) g->keys.rot_r = 1;
	return 0;
}

int key_release(int keycode, t_game *g)
{
	if (keycode == XK_w) g->keys.up = 0;
	else if (keycode == XK_s) g->keys.down = 0;
	else if (keycode == XK_a) g->keys.left = 0;
	else if (keycode == XK_d) g->keys.right = 0;
	else if (keycode == XK_Left) g->keys.rot_l = 0;
	else if (keycode == XK_Right) g->keys.rot_r = 0;
	return 0;
}

static void rotate_player(t_player *p, double angle)
{
	double old_dir_x = p->dir_x;
	p->dir_x = p->dir_x * cos(angle) - p->dir_y * sin(angle);
	p->dir_y = old_dir_x * sin(angle) + p->dir_y * cos(angle);
	double old_plane_x = p->plane_x;
	p->plane_x = p->plane_x * cos(angle) - p->plane_y * sin(angle);
	p->plane_y = old_plane_x * sin(angle) + p->plane_y * cos(angle);
}

int mouse_move(int x, int y, t_game *g)
{
    /* Use window center as a stable reference and warp cursor back each frame */
    int cx = g->width / 2;
    int cy = g->height / 2;
    if (x == cx && y == cy)
        return 0; /* ignore our own recenter event */
    int dx = x - cx; /* horizontal delta from center */
    (void)y; /* vertical ignored for now */
	if (dx != 0) {
		double angle = -dx * 0.0035; /* negative = clockwise when moving mouse to the right */
        rotate_player(&g->player, angle);
    }
    mlx_mouse_move(g->mlx, g->win, cx, cy);
    return 0;
}


int main()
{
	t_game	gm;

	if (init_game(&gm))
		return (1);
	mlx_loop_hook(gm.mlx, main_loop, &gm);
	mlx_hook(gm.win, 2, 1L<<0, key_press, &gm);      /* KeyPress */
	mlx_hook(gm.win, 3, 1L<<1, key_release, &gm);    /* KeyRelease */
	mlx_hook(gm.win, 17, 0, close_win, &gm);         /* DestroyNotify */
	mlx_hook(gm.win, MotionNotify, PointerMotionMask, mouse_move, &gm); /* Mouse move */
	mlx_loop(gm.mlx);
	return (0);
}
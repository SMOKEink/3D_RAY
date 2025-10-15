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
	gm->map = g_map;
	gm->player.x = 5.5;
	gm->player.y = 5.5;
	gm->player.dir_x = -1;
	gm->player.dir_y = 0;
	fov = 0.66;
	gm->player.plane_x = gm->player.dir_y * fov;
	gm->player.plane_y = -gm->player.dir_x * fov;
	mlx_mouse_hide(gm->mlx, gm->win);
	return (0);
}

bool	is_wall(t_game *gm, int x, int y)
{
	if (x < 0 || y < 0 || x >= gm->map_width || y >= gm->map_height)
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
		nx = p->x + p->dir_x * 0.001;
		ny = p->y + p->dir_y * 0.001;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.down)
	{
		nx = p->x - p->dir_x * 0.001;
		ny = p->y - p->dir_y * 0.001;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.right) // (y, -x)
	{
		nx = p->x + p->dir_y * 0.001;
		ny = p->y - p->dir_x * 0.001;
		if (!is_wall(gm, (int)nx, (int)p->y))
			p->x = nx;
		if (!is_wall(gm, (int)p->x, (int)ny))
			p->y = ny;
	}
	if (gm->keys.left) // (-y, x)
	{
		nx = p->x - p->dir_y * 0.001;
		ny = p->y + p->dir_x * 0.001;
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

void	render_image(t_game *gm)
{

}

int	main_loop(t_game *gm)
{
	move_player(gm);
	render_image(gm);
	return (0);
}

int main()
{
	t_game	gm;

	if (init_game(&gm))
		return (1);
	mlx_loop_hook(gm.mlx, main_loop, &gm);

	return (0);
}
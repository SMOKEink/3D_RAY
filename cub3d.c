#include "cub3d.h"

bool	is_wall(t_game *gm, int x, int y)
{
	char	c;
	int		row_width;

	if (x < 0 || y < 0 || y >= gm->map_h)
		return (1);
	row_width = (int)strlen(gm->map[y]);
	if (x >= row_width)
		return (1);
	c = gm->map[y][x];
	if (c == '1' || c == ' ' || c == 'D')
		return (1);
	return (0);
}

void	put_pixel(t_game *gm, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return ;
	dst = gm->frame.data_img + y * gm->frame.line_len + x * (gm->frame.bpp / 8);
	*(unsigned int *)dst = color;
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
	draw_hands(gm);
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
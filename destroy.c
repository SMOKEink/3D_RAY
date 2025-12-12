#include "cub3d.h"

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
#include "cub3d.h"

int load_tex_any(t_game *gm, t_tex *tex, const char *path)
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
int load_tex(t_game *gm, int idx, const char *path)
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
	if (gm->hand_path && load_tex_any(gm, &gm->hand, gm->hand_path))
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
}
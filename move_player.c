#include "cub3d.h"

static int collides_at(t_game *gm, double x, double y)
{
	const double r = 0.20;
	int x_min = (int)floor(x - r);
	int x_max = (int)floor(x + r);
	int y_min = (int)floor(y - r);
	int y_max = (int)floor(y + r);

	int ix = x_min;
	while (ix <= x_max)
	{
		int iy = y_min;
		while (iy <= y_max)
		{
			if (is_wall(gm, ix, iy))
				return 1;
			iy++;
		}
		ix++;
	}
	return 0;
}

void	move_player(t_game *gm)
{
	t_player	*p = &gm->player;
	double		nx;
	double		ny;
	bool		moving = false;

	if (gm->keys.up)
	{
		nx = p->x + p->dir_x * MOVE_SPEED;
		ny = p->y + p->dir_y * MOVE_SPEED;
		if (!collides_at(gm, nx, p->y))
			p->x = nx;
		if (!collides_at(gm, p->x, ny))
			p->y = ny;
		moving = true;
	}
	if (gm->keys.down)
	{
		nx = p->x - p->dir_x * MOVE_SPEED;
		ny = p->y - p->dir_y * MOVE_SPEED;
		if (!collides_at(gm, nx, p->y))
			p->x = nx;
		if (!collides_at(gm, p->x, ny))
			p->y = ny;
		moving = true;
	}
	if (gm->keys.right)
	{
		nx = p->x + p->dir_y * MOVE_SPEED;
		ny = p->y - p->dir_x * MOVE_SPEED;
		if (!collides_at(gm, nx, p->y))
			p->x = nx;
		if (!collides_at(gm, p->x, ny))
			p->y = ny;
		moving = true;
	}
	if (gm->keys.left)
	{
		nx = p->x - p->dir_y * MOVE_SPEED;
		ny = p->y + p->dir_x * MOVE_SPEED;
		if (!collides_at(gm, nx, p->y))
			p->x = nx;
		if (!collides_at(gm, p->x, ny))
			p->y = ny;
		moving = true;
	}
	if (gm->keys.rot_l || gm->keys.rot_r)
	{
		double old_dir_x;
		double old_plane_x;
		double angle;

		if (gm->keys.rot_r)
			angle = -ROT_SPEED;
		else
			angle = ROT_SPEED;
		old_dir_x = p->dir_x;
		p->dir_x = p->dir_x * cos(angle) - p->dir_y * sin(angle);
		p->dir_y = old_dir_x * sin(angle) + p->dir_y * cos(angle);
		old_plane_x = p->plane_x;
		p->plane_x = p->plane_x * cos(angle) - p->plane_y * sin(angle);
		p->plane_y = old_plane_x * sin(angle) + p->plane_y * cos(angle);
	}
	if (moving)
		gm->hand_phase += 0.12;
	else
		gm->hand_phase *= 0.92;
}

void rotate_player(t_player *p, double angle)
{
	double old_dir_x;
	double old_plane_x;

	old_dir_x = p->dir_x;
	p->dir_x = p->dir_x * cos(angle) - p->dir_y * sin(angle);
	p->dir_y = old_dir_x * sin(angle) + p->dir_y * cos(angle);
	
	old_plane_x = p->plane_x;
	p->plane_x = p->plane_x * cos(angle) - p->plane_y * sin(angle);
	p->plane_y = old_plane_x * sin(angle) + p->plane_y * cos(angle);
}

int mouse_move(int x, int y, t_game *g)
{
	double	angle;
	int cx = WIDTH / 2;
	int cy = HEIGHT / 2;
	if (x == cx && y == cy)
		return (0);
	(void)y;
	int dx = x - cx;
	if (dx != 0)
	{
		angle = -dx * MOUSE_ROT_SPEED;
		rotate_player(&g->player, angle);
	}
	mlx_mouse_move(g->mlx, g->win, cx, cy);
	return (0);
}
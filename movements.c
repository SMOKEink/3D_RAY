#include "cub3d.h"

static int collides_at(t_game *gm, double x, double y)
{
	double	r;
	int		x_min;
	int		x_max;
	int		y_min;
	int		y_max;
	int		ix;
	int		iy;
	
	r = 0.2;
	x_min = floor(x - r);
	y_min = floor(y - r);
	x_max = floor(x + r);
	y_max = floor(y + r);
	ix = x_min - 1;
	while (++ix <= x_max)
	{
		iy = y_min - 1;
		while (++iy <= y_max)
			if (is_wall(gm, ix, iy))
				return (1);
	}
	return (0);
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

void	move_forward(t_game *gm, t_player *p)
{
	double		new_x;
	double		new_y;
	
	new_x = p->x + p->dir_x * MOVE_SPEED;
	new_y = p->y + p->dir_y * MOVE_SPEED;
	if (!collides_at(gm, new_x, p->y))
		p->x = new_x;
	if (!collides_at(gm, p->x, new_y))
		p->y = new_y;
	p->moving = true;	
}
void	move_backward(t_game *gm, t_player *p)
{
	double		new_x;
	double		new_y;

	new_x = p->x - p->dir_x * MOVE_SPEED;
	new_y = p->y - p->dir_y * MOVE_SPEED;
	if (!collides_at(gm, new_x, p->y))
		p->x = new_x;
	if (!collides_at(gm, p->x, new_y))
		p->y = new_y;
	p->moving = true;
}
void	move_right(t_game *gm, t_player *p)
{
	double		new_x;
	double		new_y;

	new_x = p->x + p->dir_y * MOVE_SPEED;
	new_y = p->y - p->dir_x * MOVE_SPEED;
	if (!collides_at(gm, new_x, p->y))
		p->x = new_x;
	if (!collides_at(gm, p->x, new_y))
		p->y = new_y;
	p->moving = true;
}
void	move_left(t_game *gm, t_player *p)
{
	double		new_x;
	double		new_y;

	new_x = p->x - p->dir_y * MOVE_SPEED;
	new_y = p->y + p->dir_x * MOVE_SPEED;
	if (!collides_at(gm, new_x, p->y))
		p->x = new_x;
	if (!collides_at(gm, p->x, new_y))
		p->y = new_y;
	p->moving = true;
}
void	move_player(t_game *gm)
{
	t_player	*p;

	p = &gm->player;
	p->moving = false;
	if (gm->keys.up)
		move_forward(gm, p);
	if (gm->keys.down)
		move_backward(gm, p);
	if (gm->keys.right)
		move_right(gm, p);
	if (gm->keys.left)
		move_left(gm, p);
	if (gm->keys.rot_l || gm->keys.rot_r)
	{
		if (gm->keys.rot_r)
			rotate_player(p, -ROT_SPEED);
		else
			rotate_player(p, ROT_SPEED);
	}
	if (p->moving)
		gm->hand_phase += 0.12;
	else
		gm->hand_phase *= 0.92;
}

int	mouse_move(int x, int y, t_game *g)
{
	double	angle;
	int		cx;
	int		cy;
	int		delta_x;

	(void)y;
	cx = WIDTH / 2;
	cy = HEIGHT / 2;
	if (x == cx && y == cy)
		return (0);
	delta_x = x - cx;
	if (delta_x)
	{
		angle = -delta_x * MOUSE_ROT_SPEED;
		rotate_player(&g->player, angle);
	}
	mlx_mouse_move(g->mlx, g->win, cx, cy);
	return (0);
}

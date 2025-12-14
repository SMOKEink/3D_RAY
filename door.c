#include "cub3d.h"

#define DOOR_FRONT_THRESHOLD 0.5

typedef struct s_door_pick
{
	int		x;
	int		y;
	double	dist;
}	t_door_pick;

static int	in_bounds(t_game *gm, int x, int y)
{
	int	rowlen;

	if (y < 0 || y >= gm->map_h)
		return (0);
	rowlen = (int)strlen(gm->map[y]);
	if (x < 0 || x >= rowlen)
		return (0);
	return (1);
}

static int	consider_cell(t_game *gm, t_door_pick *best, int tx, int ty)
{
	char	cell;
	double	cx;
	double	cy;
	double	vx;
	double	vy;
	double	dist;
	double	dot;

	cell = gm->map[ty][tx];
	if (cell != 'D' && cell != 'd')
		return (0);
	cx = tx + 0.5;
	cy = ty + 0.5;
	vx = cx - gm->player.x;
	vy = cy - gm->player.y;
	dist = sqrt(vx * vx + vy * vy);
	if (dist < 1e-6)
		return (0);
	dot = (vx / dist) * gm->player.dir_x + (vy / dist) * gm->player.dir_y;
	if (dot < DOOR_FRONT_THRESHOLD)
		return (0);
	if (dist < best->dist)
		return (best->dist = dist, best->x = tx, best->y = ty, 0);
	return (0);
}
static void	scan_neighbours(t_game *gm, t_door_pick *best)
{
	int	px;
	int	py;
	int	dy;
	int	dx;
	int	ty;
	int	tx;

	px = (int)gm->player.x;
	py = (int)gm->player.y;
	dy = -2;
	while (++dy <= 1)
	{
		ty = py + dy;
		dx = -2;
		while (++dx <= 1)
		{
			tx = px + dx;
			if (in_bounds(gm, tx, ty))
				consider_cell(gm, best, tx, ty);
		}
	}
}
static void	apply_toggle(t_game *gm, t_door_pick *best)
{
	char	*cell;
	int	player_x;
	int	player_y;

	if (best->x == -1 || best->dist > 1.2)
		return ;
	cell = &gm->map[best->y][best->x];
	player_x = (int)gm->player.x;
	player_y = (int)gm->player.y;
	if (*cell == 'd')
	{
		if (player_x == best->x && player_y == best->y)
			return ;
		if (best->dist < 0.35)
			return ;
		*cell = 'D';
	}
	else if (*cell == 'D')
		*cell = 'd';
}
void	toggle_door(t_game *gm)
{
	t_door_pick	best;

	best.x = -1;
	best.y = -1;
	best.dist = 10.0;
	scan_neighbours(gm, &best);
	apply_toggle(gm, &best);
}
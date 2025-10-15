#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <math.h>
# include <stdio.h>
# include <string.h>
# include "minilibx-linux/mlx.h"
# include <X11/keysym.h> // For key symbols
# include <X11/X.h>      // For MotionNotify masks

typedef struct s_img
{
	void	*img;
	char	*data_img;
	int		bpp;
	int		line_len;
	int		endian;
}	t_img;

typedef	struct s_player
{
	double	x;
	double	y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
}	t_player;

typedef	struct s_keys
{
	int		up;
	int		down;
	int		right;
	int		left;
	int		rot_r;
	int		rot_l;
}	t_keys;


typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_img		frame;
	t_player	player;
	t_keys		keys;
	int			map_width;
	int			map_height;
	char		**map;
}	t_game;

#endif
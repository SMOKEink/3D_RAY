#ifndef CUB3D_H
# define CUB3D_H

# define WIDTH 1280
# define HEIGHT 720

# define MOVE_SPEED 0.08
# define ROT_SPEED 0.05
# define MOUSE_ROT_SPEED 0.00035

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <math.h>
# include <fcntl.h>
# include <sys/time.h>
# include <stdio.h>
# include <string.h>
# include "minilibx-linux/mlx.h"
# include <X11/keysym.h>
# include <X11/X.h>

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
	bool	moving;
}	t_player;

typedef	struct s_keys
{
	int		up;
	int		down;
	int		right;
	int		left;
	int		rot_r;
	int		rot_l;
	int		use;
}	t_keys;

#ifndef TEX_WALL_COUNT
# define TEX_WALL_COUNT 4
# define TEX_N 0
# define TEX_S 1
# define TEX_W 2
# define TEX_E 3
#endif

typedef struct s_tex {
	void   *img;
	char   *data;
	int     w;
	int     h;
	int     bpp;
	int     line_len;
	int     endian;
} t_tex;

#define DOOR_FRONT_THRESHOLD 0.5

typedef struct s_door_pick
{
	int		x;
	int		y;
	double	dist;
}	t_door_pick;

typedef struct s_ray
{
	double  cam_x;
	double  dir_x;
	double  dir_y;
	int     map_x;
	int     map_y;
	double  delta_x;
	double  delta_y;
	double  side_x;
	double  side_y;
	int     step_x;
	int     step_y;
	bool    hit_vertical;
	double  perp_dist;
}   t_ray;

typedef struct s_slice
{
	int     line_len;
	int     line_start;
	int     line_end;
	double  tex_step;
	double  tex_pos;
	int     tex_x;
	int     tex_y;
	int		column;
}   t_slice;

typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_img		frame;
	t_player	player;
	t_keys		keys;
	int			map_w;
	int			map_h;
	char		**map;
	char		*filename;
	int			ceiling;
	int			floor;
	char		*tex_path[TEX_WALL_COUNT];
	t_tex   	wall[TEX_WALL_COUNT];
	char		*door_path;
	t_tex		door;
	char		*hand_path;
	t_tex		hand;
	double		hand_phase;
}	t_game;

//COLORS
// const unsigned int col_bg    = color(24, 24, 28);
// 	const unsigned int col_ring  = color(255, 255, 255);
// 	const unsigned int col_wall  = color(0, 0, 0);
// 	const unsigned int col_door  = color(100, 255, 100);
// 	const unsigned int col_door_open = color(40, 120, 40);
// 	const unsigned int col_player= color(255, 60, 60);
// 	const unsigned int col_fov   = color(255, 100, 100);
# define COL_ROAD 			1579036
# define COL_RING 			16777215
# define COL_WALL 			0
# define COL_DOOR 			6618980
# define COL_OPENED_DOOR	2652200
# define COL_PLAYER			16727100
# define COL_DIRECTION		16737380

#define MMAP_X				80
#define MMAP_Y				80
#define MMAP_R				70
#define MMAP_PIX_PER_CELL	7

//Hands
#define HAND_HEIGHT_PX		900
#define HAND_BOB_PIX		14
#define HAND_CROP_TOP_PCT	30
#define HAND_OFFSET_Y		30

void			toggle_door(t_game *gm);
int				load_tex_any(t_game *gm, t_tex *tex, const char *path);
void			free_textures(t_game *gm);

int				parse_config(t_game *gm, const char *filename);
void			render_image(t_game *gm);
bool			is_wall(t_game *gm, int x, int y);
void			put_pixel(t_game *gm, int x, int y, int color);
int				close_win(t_game *g);
int				mouse_move(int x, int y, t_game *g);
void			rotate_player(t_player *p, double angle);
int				key_press(int keycode, t_game *g);
int				key_release(int keycode, t_game *g);
void			move_player(t_game *gm);

void			free_map(char **map);
int				load_textures(t_game *gm);
void			draw_minimap(struct s_game *gm);
unsigned int	texel_at(t_tex *t, int x, int y);
int				set_player_spawn(t_game *gm);
void			destroy_game(t_game *g, const char *msg);
void			draw_hands(t_game *gm);

#endif
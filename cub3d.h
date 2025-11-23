#ifndef CUB3D_H
# define CUB3D_H

# define WIDTH 1280
# define HEIGHT 720
# define MOVE_SPEED 0.04
# define ROT_SPEED 0.05
# define MOUSE_ROT_SPEED 0.00035

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <math.h>
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
}	t_game;


#define MM_RADIUS_PX      75     // pixel radius of minimap circle
#define MM_PIX_PER_CELL    8     // pixels per world cell on the minimap
#define MM_MARGIN_X       12     // offset from screen edge
#define MM_MARGIN_Y       12

int		parse_config(t_game *gm, const char *filename);
void	render_image(t_game *gm);
bool	is_wall(t_game *gm, int x, int y);
void	put_pixel(t_game *gm, int x, int y, int color);
int		close_win(t_game *g);
int		mouse_move(int x, int y, t_game *g);
void	rotate_player(t_player *p, double angle);
int		key_press(int keycode, t_game *g);
int		key_release(int keycode, t_game *g);
void	move_player(t_game *gm);
bool	is_solid_for_ray(t_game *gm, int x, int y);

void	free_map(char **map);
int		load_textures(t_game *gm);
void 	free_textures(t_game *gm);
void	draw_minimap(struct s_game *gm);
unsigned int texel_at(t_tex *t, int x, int y);
bool    inside_portal(t_game *gm, int map_x, int map_y, double side_distx, double side_disty, double ray_dir_x, double ray_dir_y, int x, int y);

#endif
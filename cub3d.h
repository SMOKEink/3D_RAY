#ifndef CUB3D_H
# define CUB3D_H

# define WIDTH 1280
# define HEIGHT 720
# define MOVE_SPEED 0.035
# define ROT_SPEED 0.05
# define MOUSE_ROT_SPEED 0.00035

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
// # include <ctype.h>
# include <math.h>
# include <sys/time.h>
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
	int		use;
	// int		sprint;
}	t_keys;

#ifndef TEX_WALL_COUNT
# define TEX_WALL_COUNT 4
# define TEX_N 0
# define TEX_S 1
# define TEX_W 2
# define TEX_E 3
/* --- Door animation (GIF frames as XPMs) ----------------------------- */

/* Forward declaration to avoid dependency on typedef order */
struct s_tex;
struct s_game;

/* Advances time for door animation (dt<=0 computes internally) */
void        update_door_anim(struct s_game *gm, double dt);
/* Get current door texture frame; falls back to gm->door if none loaded */
struct s_tex *door_anim_current(struct s_game *gm);
/* Optional: preload frames with a prefix like "textures/door/a3_" */
int         load_door_anim(struct s_game *gm, const char *prefix);
void        free_door_anim(struct s_game *gm);

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

#define DOOR_MAX_FRAMES 20
typedef struct s_anim {
    t_tex   frames[DOOR_MAX_FRAMES];
    int     count;
    double  fps;
    double  taccum;   // seconds
} t_anim;

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
	t_anim		door_anim;
}	t_game;

int   load_door_anim(t_game *gm, const char *prefix); // e.g. "textures/door/a3_"
void  free_door_anim(t_game *gm);
void  update_door_anim(t_game *gm, double dt);
t_tex *door_anim_current(t_game *gm);


#define MM_RADIUS_PX      75     // pixel radius of minimap circle
#define MM_PIX_PER_CELL    8     // pixels per world cell on the minimap
#define MM_MARGIN_X       12     // offset from screen edge
#define MM_MARGIN_Y       12

// // HUD hands
// #define HAND_COLKEY       0xFF00FF       // magenta chroma-key (transparent)
// #define HAND_IDLE_MAX     8              // max idle frames supported
// #define HAND_ATK_MAX      8              // max attack frames supported
// #define HAND_IDLE_FPS     8.0
// #define HAND_ATK_FPS      20.0
// #define HAND_BOB_AMP_STAND  4.0          // pixels
// #define HAND_BOB_AMP_WALK   7.0
// #define HAND_BOB_AMP_SPRINT 11.0
// #define HAND_BOB_FREQ_STAND 2.0          // Hz
// #define HAND_BOB_FREQ_MOVE  4.0
// #define HAND_BOB_FREQ_SPRINT 6.5
// #define HAND_SCALE         1.0  

// typedef enum e_hand_state {
//     HAND_STATE_IDLE = 0,
//     HAND_STATE_ATTACK = 1
// } t_hand_state;

// typedef struct s_hands {
//     // frames
//     t_tex idle[HAND_IDLE_MAX];
//     int   idle_count;
//     t_tex attack[HAND_ATK_MAX];
//     int   attack_count;
//     // animation
//     t_hand_state state;
//     int   frame;
//     double frame_accum; // seconds accumulator
//     // bobbing
//     double bob_t;       // time accumulator for bobbing (seconds)
//     // runtime
//     int    attack_latched; // avoid retrigger while held
// 	double bob_amp;   // NEW: current amplitude to use in draw
// } t_hands;




// Parser functions
int	parse_config(t_game *gm, const char *filename);
void	render_image(t_game *gm);
bool	is_wall(t_game *gm, int x, int y);
void	put_pixel(t_game *gm, int x, int y, int color);
int close_win(t_game *g);
int mouse_move(int x, int y, t_game *g);
void rotate_player(t_player *p, double angle);
int key_press(int keycode, t_game *g);
int key_release(int keycode, t_game *g);
void	move_player(t_game *gm);
bool	is_solid_for_ray(t_game *gm, int x, int y);

void	free_map(char **map);
int   load_textures(t_game *gm);
void  free_textures(t_game *gm);
void draw_minimap(struct s_game *gm);
// Hands API
int  load_hand_textures(t_game *gm);
void free_hand_textures(t_game *gm);
void update_hands(t_game *gm, double dt);
void draw_hands(t_game *gm);


#endif
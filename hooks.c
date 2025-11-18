#include "cub3d.h"

int key_press(int keycode, t_game *g)
{
	if (keycode == XK_Escape)
		close_win(g);
	else if (keycode == XK_w)
		g->keys.up = 1;
	else if (keycode == XK_s)
		g->keys.down = 1;
	else if (keycode == XK_a)
		g->keys.left = 1;
	else if (keycode == XK_d)
		g->keys.right = 1;
	else if (keycode == XK_Left)
		g->keys.rot_l = 1;
	else if (keycode == XK_Right)
		g->keys.rot_r = 1;
	// else if (keycode == XK_Shift_L || keycode == XK_Shift_R)
    //     g->keys.sprint = 1;
	return 0;
}

int key_release(int keycode, t_game *g)
{
	if (keycode == XK_w)
		g->keys.up = 0;
	else if (keycode == XK_s)
		g->keys.down = 0;
	else if (keycode == XK_a)
		g->keys.left = 0;
	else if (keycode == XK_d)
		g->keys.right = 0;
	else if (keycode == XK_Left)
		g->keys.rot_l = 0;
	else if (keycode == XK_Right)
		g->keys.rot_r = 0;
	// else if (keycode == XK_Shift_L || keycode == XK_Shift_R)
    //     g->keys.sprint = 0;
	return 0;
}

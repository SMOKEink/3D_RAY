#include "cub3d.h"

void toggle_door(t_game *gm)
{
    int px = (int)gm->player.x;
    int py = (int)gm->player.y;
    double best_dist = 10.0;
    int best_x = -1, best_y = -1;

    double dirx = gm->player.dir_x;
    double diry = gm->player.dir_y;
    const double front_threshold = 0.5;

    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            int tx = px + dx;
            int ty = py + dy;
            if (ty < 0 || ty >= gm->map_h)
				continue;
            int rowlen = (int)strlen(gm->map[ty]);
            if (tx < 0 || tx >= rowlen)
				continue;
            char c = gm->map[ty][tx];
            if (c != 'D' && c != 'd')
                continue;
            double cx = tx + 0.5;
            double cy = ty + 0.5;
            double vx = cx - gm->player.x;
            double vy = cy - gm->player.y;
            double dist = sqrt(vx*vx + vy*vy);
            if (dist < 1e-6)
				continue;
            double dot = (vx/dist) * dirx + (vy/dist) * diry;
            if (dot < front_threshold)
                continue;
            if (dist < best_dist)
            {
                best_dist = dist;
                best_x = tx;
                best_y = ty;
            }
        }
    }
    if (best_x == -1 || best_dist > 1.2)
        return;
    char *cell = &gm->map[best_y][best_x];
    int player_tx = (int)gm->player.x;
    int player_ty = (int)gm->player.y;
    if (*cell == 'd')
    {
        if (player_tx == best_x && player_ty == best_y)
            return;
        if (best_dist < 0.35)
            return;
        *cell = 'D';
    }
    else if (*cell == 'D')
    {
        *cell = 'd';
    }
}
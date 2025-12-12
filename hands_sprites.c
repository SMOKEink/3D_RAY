#include "cub3d.h"

void	draw_hands(t_game *gm)
{
    if (!gm->hand.img || !gm->hand.data)
        return;
    int crop_top = (gm->hand.h * HAND_CROP_TOP_PCT) / 100;
    if (crop_top < 0) crop_top = 0;
    if (crop_top >= gm->hand.h) crop_top = gm->hand.h - 1;
    int crop_h = gm->hand.h - crop_top;

    int target_h = HAND_HEIGHT_PX;
    if (target_h > HEIGHT - 20) target_h = HEIGHT - 20;
    if (target_h < 40) target_h = 40;
    int target_w = (int)((double)gm->hand.w * (double)target_h / (double)crop_h);

    int bob = (int)(sin(gm->hand_phase) * HAND_BOB_PIX);
    int base_x = (WIDTH - target_w) / 2;
    int base_y = HEIGHT - target_h - 8 + HAND_OFFSET_Y + bob;
    for (int ty = 0; ty < target_h; ++ty)
    {
        int sy = crop_top + (ty * crop_h / target_h);
        int py = base_y + ty;
        if (py < 0 || py >= HEIGHT) continue;
        for (int tx = 0; tx < target_w; ++tx)
        {
            int sx = tx * gm->hand.w / target_w;
            int px = base_x + tx;
            if (px < 0 || px >= WIDTH) continue;
            unsigned int col = texel_at(&gm->hand, sx, sy);
            int r = (col >> 16) & 0xFF;
            int g = (col >> 8) & 0xFF;
            int b = col & 0xFF;
            if (r + g + b < 45) continue; // treat dark (black square) as transparent
            put_pixel(gm, px, py, col);
        }
    }
}
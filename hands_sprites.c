#include "cub3d.h"

typedef struct s_hand_ctx
{
    int	crop_top;
    int	crop_h;
    int	target_w;
    int	target_h;
    int	base_x;
    int	base_y;
}t_hand_ctx;

static void	init_crop(t_game *gm, t_hand_ctx *ctx)
{
    ctx->crop_top = (gm->hand.h * HAND_CROP_TOP_PCT) / 100;
    if (ctx->crop_top < 0)
        ctx->crop_top = 0;
    if (ctx->crop_top >= gm->hand.h)
        ctx->crop_top = gm->hand.h - 1;
    ctx->crop_h = gm->hand.h - ctx->crop_top;
    if (ctx->crop_h <= 0)
        ctx->crop_h = 1;
}

static void	init_target_size(t_game *gm, t_hand_ctx *ctx)
{
    ctx->target_h = HAND_HEIGHT_PX;
    if (ctx->target_h > HEIGHT - 20)
        ctx->target_h = HEIGHT - 20;
    if (ctx->target_h < 40)
        ctx->target_h = 40;
    ctx->target_w = (int)((double)gm->hand.w * (double)ctx->target_h
            / (double)ctx->crop_h);
    if (ctx->target_w <= 0)
        ctx->target_w = 1;
}

static void	init_hand_base(t_game *gm, t_hand_ctx *ctx)
{
    int	bob;

    bob = (int)(sin(gm->hand_phase) * HAND_BOB_PIX);
    ctx->base_x = (WIDTH - ctx->target_w) / 2;
    ctx->base_y = HEIGHT - ctx->target_h - 8 + HAND_OFFSET_Y + bob;
}

static unsigned int	sample_hand_color(t_game *gm, t_hand_ctx *ctx, int tx, int ty)
{
    int			sx;
    int			sy;
    unsigned int	col;
    int			sum;

    sy = ctx->crop_top + (ty * ctx->crop_h / ctx->target_h);
    sx = tx * gm->hand.w / ctx->target_w;
    col = texel_at(&gm->hand, sx, sy);
    sum = ((col >> 16) & 0xFF) + ((col >> 8) & 0xFF) + (col & 0xFF);
    if (sum < 45)
        return (0);
    return (col);
}

static void	draw_hand_row(t_game *gm, t_hand_ctx *ctx, int ty)
{
    int			px;
    int			tx;
    unsigned int	col;
    int			py;

    py = ctx->base_y + ty;
    if (py < 0 || py >= HEIGHT)
        return ;
    tx = 0;
    while (tx < ctx->target_w)
    {
        px = ctx->base_x + tx;
        if (px >= 0 && px < WIDTH)
        {
            col = sample_hand_color(gm, ctx, tx, ty);
            if (col)
                put_pixel(gm, px, py, col);
        }
        tx++;
    }
}

static void	draw_scaled_hand(t_game *gm, t_hand_ctx *ctx)
{
    int	ty;

    ty = 0;
    while (ty < ctx->target_h)
    {
        draw_hand_row(gm, ctx, ty);
        ty++;
    }
}

void	draw_hands(t_game *gm)
{
    t_hand_ctx	ctx;

    if (!gm->hand.img || !gm->hand.data)
        return ;
    init_crop(gm, &ctx);
    init_target_size(gm, &ctx);
    init_hand_base(gm, &ctx);
    if (ctx.target_w <= 0 || ctx.target_h <= 0)
        return ;
    draw_scaled_hand(gm, &ctx);
}
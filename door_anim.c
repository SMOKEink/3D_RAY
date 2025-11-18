#include "cub3d.h"
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

typedef struct s_anim_state {
    t_tex   frames[DOOR_MAX_FRAMES];
    int     count;
    double  fps;
    double  taccum;
    int     loaded;
}   t_anim_state;

static t_anim_state g_door_anim = { .count = 0, .fps = 12.0, .taccum = 0.0, .loaded = 0 };

static double now_seconds(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

int load_door_anim(t_game *gm, const char *prefix)
{
    (void)gm;
    // clean any previous
    for (int i = 0; i < g_door_anim.count; ++i) {
        if (g_door_anim.frames[i].img && gm && gm->mlx)
            mlx_destroy_image(gm->mlx, g_door_anim.frames[i].img);
        memset(&g_door_anim.frames[i], 0, sizeof(t_tex));
    }
    g_door_anim.count = 0;
    g_door_anim.taccum = 0.0;

    if (!gm || !gm->mlx || !prefix)
        return 1;

    char path[256];
    for (int i = 0; i < DOOR_MAX_FRAMES; ++i) {
        snprintf(path, sizeof(path), "%s%02d.xpm", prefix, i);
        t_tex *t = &g_door_anim.frames[i];
        t->img = mlx_xpm_file_to_image(gm->mlx, path, &t->w, &t->h);
        if (!t->img) break; // stop when frame missing
        t->data = mlx_get_data_addr(t->img, &t->bpp, &t->line_len, &t->endian);
        if (!t->data) {
            mlx_destroy_image(gm->mlx, t->img);
            memset(t, 0, sizeof(*t));
            break;
        }
        g_door_anim.count++;
    }
    g_door_anim.loaded = 1;
    return (g_door_anim.count == 0);
}

void free_door_anim(t_game *gm)
{
    if (!gm) return;
    for (int i = 0; i < g_door_anim.count; ++i) {
        if (g_door_anim.frames[i].img && gm->mlx)
            mlx_destroy_image(gm->mlx, g_door_anim.frames[i].img);
        memset(&g_door_anim.frames[i], 0, sizeof(t_tex));
    }
    g_door_anim.count = 0;
    g_door_anim.taccum = 0.0;
    g_door_anim.loaded = 0;
}

void update_door_anim(t_game *gm, double dt)
{
    (void)gm;
    static double last = 0.0;
    if (dt <= 0.0) {
        double now = now_seconds();
        dt = (last == 0.0) ? 0.0 : (now - last);
        last = now;
    }
    if (g_door_anim.count > 0)
        g_door_anim.taccum += dt;
}

t_tex *door_anim_current(t_game *gm)
{
    // Lazy load once if never tried
    if (!g_door_anim.loaded) {
        // Default prefix; user can call load_door_anim with custom later
        load_door_anim(gm, "textures/door/a3_");
    }
    if (g_door_anim.count <= 0)
        return &gm->door; // fallback to static door texture
    int idx = (int)(g_door_anim.taccum * g_door_anim.fps);
    if (idx < 0) idx = 0;
    idx %= g_door_anim.count;
    return &g_door_anim.frames[idx];
}

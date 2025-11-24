#include "cub3d.h"
#include <fcntl.h>

static char	*read_line(int fd)
{
    char	buffer[2000];
    int		i;
    int		ret;
    char	*line;

    i = 0;
    while ((ret = read(fd, &buffer[i], 1)) > 0)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
            break;
        }
        i++;
        if (i >= 1999)
            break;
    }
    if (ret <= 0 && i == 0)
        return (NULL);
    buffer[i] = '\0';
    line = malloc(i + 1);
    if (!line)
        return (NULL);
    strcpy(line, buffer);
    return (line);
}

static int	is_map_line(const char *s)
{
    int i = 0;
    if (!s || !s[0]) return 0;
    while (s[i] == ' ') i++;
    if (s[i] == '1' || s[i] == '0')
        return 1;
    return 0;
}

static int	parse_rgb(const char *s, int *out_color)
{
    int i = 0;
    int r = -1, g = -1, b = -1;
    long v;
    int digits;
    int *dst[3];
    dst[0] = &r; dst[1] = &g; dst[2] = &b;
    int comp = 0;

    while (s[i] == ' ' || s[i] == '\t') i++;
    while (comp < 3)
    {
        v = 0;
        digits = 0;
        while (s[i] >= '0' && s[i] <= '9')
        {
            v = v * 10 + (s[i] - '0');
            i++;
            digits = 1;
        }
        if (!digits || v < 0 || v > 255)
            return 1;
        *dst[comp] = (int)v;
        comp++;
        if (comp < 3)
        {
            if (s[i] != ',')
                return 1;
            i++;
        }
    }
    while (s[i] == ' ' || s[i] == '\t') i++;
    if (s[i] != '\0')
        return 1;
    *out_color = (r << 16) | (g << 8) | b;
    return 0;
}

static int	set_texture_path(t_game *gm, int idx, const char *line, int keylen)
{
    int i = keylen;
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (!line[i])
        return 1;
    gm->tex_path[idx] = strdup(line + i);
    if (!gm->tex_path[idx])
        return 1;
    return 0;
}

int	parse_config(t_game *gm, const char *filename)
{
    int  fd;
    char *line;
    int  got_tex[TEX_WALL_COUNT];
    int  got_floor = 0;
    int  got_ceiling = 0;
    char **map = NULL;
    int  map_cap = 0;
    int  map_len = 0;
    int  header_done = 0;

    int i_init = 0;
    while (i_init < TEX_WALL_COUNT)
    {
        got_tex[i_init] = 0;
        i_init++;
    }
    gm->ceiling = -1;
    gm->floor = -1;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
        return 1;

    while ((line = read_line(fd)))
    {
        int i = 0;
        while (line[i] == ' ' || line[i] == '\t') i++;

        if (!header_done &&
            (strncmp(line + i, "NO ", 3) == 0 || strncmp(line + i, "SO ", 3) == 0 ||
             strncmp(line + i, "WE ", 3) == 0 || strncmp(line + i, "EA ", 3) == 0))
        {
            if (line[i] == 'N' && line[i+1] == 'O' && !got_tex[TEX_N])
            {
                if (set_texture_path(gm, TEX_N, line + i + 2, 1) || (got_tex[TEX_N] = 1, 0))
                    return (free(line), close(fd), 1);
            }
            else if (line[i] == 'S' && line[i+1] == 'O' && !got_tex[TEX_S])
            {
                if (set_texture_path(gm, TEX_S, line + i + 2, 1) || (got_tex[TEX_S] = 1, 0))
                    return (free(line), close(fd), 1);
            }
            else if (line[i] == 'W' && line[i+1] == 'E' && !got_tex[TEX_W])
            {
                if (set_texture_path(gm, TEX_W, line + i + 2, 1) || (got_tex[TEX_W] = 1, 0))
                    return (free(line), close(fd), 1);
            }
            else if (line[i] == 'E' && line[i+1] == 'A' && !got_tex[TEX_E])
            {
                if (set_texture_path(gm, TEX_E, line + i + 2, 1) || (got_tex[TEX_E] = 1, 0))
                    return (free(line), close(fd), 1);
            }
            else
            {
                return (free(line), close(fd), 1);
            }
        }
        else if (!header_done && line[i] == 'F' && (line[i+1] == ' ' || line[i+1] == '\t') && !got_floor)
        {
            if (parse_rgb(line + i + 1, &gm->floor))
                return (free(line), close(fd), 1);
            got_floor = 1;
        }
        else if (!header_done && line[i] == 'C' && (line[i+1] == ' ' || line[i+1] == '\t') && !got_ceiling)
        {
            if (parse_rgb(line + i + 1, &gm->ceiling))
                return (free(line), close(fd), 1);
            got_ceiling = 1;
        }
        else if (is_map_line(line + i))
        {
            header_done = 1;
            if (map_len + 1 >= map_cap)
            {
                int new_cap = (map_cap == 0) ? 16 : map_cap * 2;
                char **new_map = (char **)realloc(map, sizeof(char *) * (new_cap));
                if (!new_map)
                    return (free(line), close(fd), 1);
                map = new_map;
                map_cap = new_cap;
            }
            map[map_len++] = line;
            line = NULL; // consumed
        }
        if (line)
            free(line);
    }

    close(fd);
	if (!gm->door_path)
        gm->door_path = strdup("textures/door_redr.xpm");
    if (!gm->hand_path)
        gm->hand_path = strdup("textures/hands.xpm");
    if (map_len == 0)
        return 1;
    map = realloc(map, sizeof(char *) * (map_len + 1));
    if (!map)
        return 1;
    map[map_len] = NULL;
    gm->map = map;
    gm->map_h = map_len;
    int chk = 0;
    while (chk < TEX_WALL_COUNT)
    {
        if (!got_tex[chk])
            return 1;
        chk++;
    }
    if (!got_floor || !got_ceiling || gm->floor < 0 || gm->ceiling < 0)
        return 1;
    return 0;
}

void	free_map(char **map)
{
    int	i;

    if (!map)
        return;
    i = 0;
    while (map[i])
    {
        free(map[i]);
        i++;
    }
    free(map);
}
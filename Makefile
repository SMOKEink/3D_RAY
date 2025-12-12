NAME = cub3d

SRC =	cub3d.c \
		parser.c \
		rendering.c \
		hooks.c \
		movements.c \
		minimap.c \
		hands_sprites.c \
		door.c \
		textures.c \
		destroy.c \
		set_player_spawn.c

OBJ = $(SRC:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -g -O3

MLX_DIR = minilibx-linux
MLX = minilibx-linux/libmlx_Linux.a
MLX_FLAGS = -L$(MLX_DIR) -lmlx_Linux -L/usr/lib -lXext -lX11 -lm -lz

all : $(NAME)

$(NAME): $(OBJ) $(MLX)
	$(CC) $(CFLAGS) $(OBJ) $(MLX_FLAGS) -o $@

%.o: %.c cub3d.h
	$(CC) $(CFLAGS) -I. -I$(MLX_DIR) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY : clean
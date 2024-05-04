#include "2048.h"
#include "libft/libft.h"
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#define BOX_DRAWING_VERTICAL "│"
#define BOX_DRAWING_HORIZONTAL "─"
#define BOX_DRAWING_TOP_LEFT "┌"
#define BOX_DRAWING_TOP_RIGHT "┐"
#define BOX_DRAWING_BOTTOM_LEFT "└"
#define BOX_DRAWING_BOTTOM_RIGHT "┘"

#define FONT_ASPECT_RATIO 2
#define DIMENSION 4

#define BRIGHT 1000

t_board	*init_board(void)
{
	t_board	*board;
	int		i;
	int		j;
	int		v;

	board = malloc(sizeof(*board));
	board->dim = DIMENSION;
	board->cells = malloc(sizeof(*board->cells) * (size_t)board->dim);
	i = -1;
	v = 1;
	while (++i < board->dim)
	{
		board->cells[i] = malloc(sizeof(*board->cells[i]) * (size_t)board->dim);
		j = -1;
		while (++j < board->dim)
			board->cells[i][j] = 0 /* 1 << v++ */;
	}
	return (board);
}

void	print_border_top(int x, int y, int cell_dim)
{
	int	d;

	mvprintw(y, x, BOX_DRAWING_TOP_LEFT);
	d = 0;
	while (++d < 2 * cell_dim - 1)
		mvprintw(y, x + d, BOX_DRAWING_HORIZONTAL);
	mvprintw(y, x + 2 * cell_dim - 1, BOX_DRAWING_TOP_RIGHT);
}

void	print_border_middle(int x, int y, int cell_dim)
{
	int	d;

	d = 0;
	while (++d < cell_dim - 1)
	{
		mvprintw(y + d, x, BOX_DRAWING_VERTICAL);
		mvprintw(y + d, x + 2 * cell_dim - 1, BOX_DRAWING_VERTICAL);
	}
}

void	print_border_bottom(int x, int y, int cell_dim)
{
	int	d;

	mvprintw(y + cell_dim - 1, x, BOX_DRAWING_BOTTOM_LEFT);
	d = 0;
	while (++d < 2 * cell_dim - 1)
		mvprintw(y + cell_dim - 1, x + d, BOX_DRAWING_HORIZONTAL);
	mvprintw(y + cell_dim - 1, x + 2 * cell_dim - 1, BOX_DRAWING_BOTTOM_RIGHT);
}

void	print_border(t_board *board, int i, int j, int cell_dim)
{
	int	x;
	int	y;

	y = i * cell_dim;
	x = j * 2 * cell_dim + COLS / 2 - board->dim * cell_dim;
	attron(COLOR_PAIR(1));
	print_border_top(x, y, cell_dim);
	print_border_middle(x, y, cell_dim);
	print_border_bottom(x, y, cell_dim);
	attroff(COLOR_PAIR(1));
}

int	max_width(char *lines_str)
{
	char	**lines;
	int		max_width = 0;

	lines = ft_split(lines_str, '\n');
	while (*lines)
		max_width = ft_max(max_width, (int)ft_utf_8_strlen(*lines++));
	return (max_width);
}

int	print_number_char(int num, int x, int y, int cell_dim, int num_len)
{
	if (num_len > FONT_ASPECT_RATIO * cell_dim - 1 || cell_dim - 2 < 0)
		return (0);
	mvprintw(y + cell_dim / 2, x + FONT_ASPECT_RATIO * cell_dim / 2 - num_len / 2, "%d", num);
	return (1);
}

int	print_numbers_height_n(int num, int x, int y, int cell_dim, int height, char *powers)
{
	char	**small_numbers;
	char	*tmp;
	int		lb = 1;
	int		num_len;
	char	**lines;
	int		line_idx = -1;

	if (cell_dim - 2 < height || num > 2147483647) /* ascii art only until 2^30. 2^31-1 will be rounded to 2^30. */
		return (0);
	tmp = ft_replace_all(powers, "NEWLINE\n", "X");
	small_numbers = ft_split(tmp, 'X');
	free(tmp);
	while (num >>= 1)
		++lb;
	num_len = max_width(small_numbers[lb]);
	if (num_len > FONT_ASPECT_RATIO * cell_dim - 2)
		return (0);
	lines = ft_split(small_numbers[lb], '\n');
	while (++line_idx < height)
		mvprintw(y + cell_dim / 2 - height / 2 + line_idx, x + FONT_ASPECT_RATIO * cell_dim / 2 - num_len / 2, "%s", lines[line_idx]);
	return (1);

}

void	fill_inside_cell(int x, int y, int cell_dim)
{
	int	i;
	int	j;

	i = 0;
	while (++i < cell_dim - 1)
	{
		j = 0;
		while (++j < cell_dim * FONT_ASPECT_RATIO - 1)
			mvprintw(y + i, x + j, " ");
	}
}

/* ret == 0 == no error */
int	print_number(t_board *board, int i, int j, int cell_dim)
{
	int	x;
	int	y;
	int	num;
	int	num_len;
	int	color;
	int	ret;

	y = i * cell_dim;
	x = j * FONT_ASPECT_RATIO * cell_dim + COLS / 2 - board->dim * cell_dim;
	num = board->cells[i][j];
	if (num < 2) /* simplify! */
		color = 2;
	else if (num < 4)
		color = 3;
	else if (num < 8)
		color = 4;
	else if (num < 16)
		color = 5;
	else if (num < 32)
		color = 6;
	else if (num < 64)
		color = 7;
	else if (num < 128)
		color = 8;
	else if (num < 256)
		color = 9;
	else if (num < 512)
		color = 10;
	else if (num < 1024)
		color = 11;
	else if (num < 2048)
		color = 12;
	else if (num < 4096)
		color = 13;
	else
		color = 14;
	attron(COLOR_PAIR(color));
	fill_inside_cell(x, y, cell_dim);
	num_len = (int)ft_strlen(ft_itoa(num));
	ret = 1;
	if (print_numbers_height_n(num, x, y, cell_dim, 16, DOH_NUMBERS))
		ret = 0;
	else if (print_numbers_height_n(num, x, y, cell_dim, 8, DOSREBEL_NUMBERS))
		ret = 0;
	else if (print_numbers_height_n(num, x, y, cell_dim, 6, SHADOW_NUMBERS))
		ret = 0;
	else if (print_numbers_height_n(num, x, y, cell_dim, 4, SMALL_NUMBERS))
		ret = 0;
	else if (print_numbers_height_n(num, x, y, cell_dim, 3, MINI_NUMBERS))
		ret = 0;
	else if (print_number_char(num, x, y, cell_dim, num_len))
		ret = 0;
	attroff(COLOR_PAIR(color));
	return (ret);
}

int	print_cell(t_board *board, int i, int j, int cell_dim)
{
	int	ret;

	print_border(board, i, j, cell_dim);
	ret = print_number(board, i, j, cell_dim);
	return (ret);
}

void	print_tty_too_small(void)
{
	clear();
	mvprintw(0, 0, "Terminal too small!\nResize it or zoom out!");
	refresh();
}

int	print_board(t_board *board)
{
	int	i;
	int	j;
	int	cell_dim;

	if (LINES * FONT_ASPECT_RATIO > COLS)
		cell_dim = COLS / (FONT_ASPECT_RATIO * board->dim);
	else
		cell_dim = LINES / board->dim;
	i = -1;
	while (++i < board->dim)
	{
		j = -1;
		while (++j < board->dim)
			if (print_cell(board, i, j, cell_dim))
				return (print_tty_too_small(), 1);
	}
	return (0);
}

void	right(t_board *board)
{
	int	i;
	int	j;

	i = -1;
	while (++i < board->dim)
	{
		j = -1;
		while (++j < board->dim)
			board->cells[i][j]++;
	}
}

void	left(t_board *board)
{
	int	i;
	int	j;

	i = -1;
	while (++i < board->dim)
	{
		j = -1;
		while (++j < board->dim)
			board->cells[i][j]--;
	}
}

/* 0 <= BRIGHT <= 1000 */
int	my_init_color(short color, int r, int g, int b)
{
	return (init_color(
		color,
		(short)(r * BRIGHT / 255),
		(short)(g * BRIGHT / 255),
		(short)(b * BRIGHT / 255)
	));
}

void	ft_setenv(char **envp, const char *name, const char *value)
{
	char	**parts;

	while (*envp)
	{
		parts = ft_split(*envp, '=');
		if (!ft_strcmp(parts[0], name))
		{
			*envp = ft_strjoin(name, ft_strjoin("=", value));
			break ;
		}
		++envp;
	}
}

void	outputPos(char *str, t_pos pos)
{
	ft_printf("%s", str);
	ft_printf("x = %d, y = %d\n", pos.x, pos.y);
}

int	two_or_four(void)
{
	srand(time(NULL));
	unsigned int two_or_four = rand();
	if(two_or_four % 5 == 0)
		two_or_four = 4;
	else
		two_or_four = 2;
	return (two_or_four);
}

void	setZeroAmount(t_board *board)
{
	board->zero_amount = 0;
	for(int i = 0; i < board->dim; i++)
	{
		for(int j = 0; j < board->dim; j++)
		{
			if(board->cells[i][j] == 0)
				board->zero_amount++;
		}
	}
}

t_pos	findFirstZero(t_board *board, t_pos start)
{
	t_pos	found;
	int	i = start.x;
	int j;

	while(i < board->dim)
	{
		if(i == start.x)
			j = start.y;
		else
			j = 0;
		while(j < board->dim)
		{
			if(board->cells[i][j] == 0)
				return(found.x = i, found.y = j, found);
			j++;
		}
		i++;
	}
	return(found.x = -1, found.y = -1, found);
}

t_pos	incrementPosition(t_board *board, t_pos pos)
{
	if(pos.y < board->dim -1)
		return (pos.y++, pos);
	pos.x++;
	if(pos.x >= board->dim)
		return(pos.x = 0, pos.y = 0, pos);
	pos.y = 0;
	return(pos);
}

t_pos	getRandomZeroPos(t_board *board)
{
	t_pos	ret;
	ret.x = 0;
	ret.y = 0;
	unsigned int	randomNum = rand();

	setZeroAmount(board);
	if(board->zero_amount <= 0)
		return (ret.x = -1, ret.y = -1, ret);
	unsigned int pos = randomNum % (board->zero_amount) +1;
	for (int i = 0; i < (int)pos; i++)
	{
		ret = findFirstZero(board, ret);
		if(i == (int)pos -1)
			break;
		ret = incrementPosition(board, ret);
	}
	return (ret);
}

void initPosition(t_board *board, t_pos pos)
{
	board->cells[pos.x][pos.y] = two_or_four();
}

void	init_colors(void)
{
	my_init_color(10, 187,173,160); /* border */
	my_init_color(11, 205,192,181); /* bg */
	my_init_color(12, 119,110,101); /* 2 fg */
	my_init_color(13, 238,229,219); /* 2 bg */
	my_init_color(14, 119,110,101); /* 4 fg */
	my_init_color(15, 236,224,201); /* 4 bg */
	my_init_color(16, 249,247,243); /* 8 fg */
	my_init_color(17, 242,177,121); /* 8 bg */
	my_init_color(18, 249,247,243); /* 16 fg */
	my_init_color(19, 244,149,98); /* 16 bg */
	my_init_color(20, 249,247,243); /* 32 fg */
	my_init_color(21, 247,125,96); /* 32 bg */
	my_init_color(22, 249,247,243); /* 64 fg */
	my_init_color(23, 246,94,59); /* 64 bg */
	my_init_color(24, 249,247,243); /* 128 fg */
	my_init_color(25, 236,206,115); /* 128 bg */
	my_init_color(26, 249,247,243); /* 256 fg */
	my_init_color(27, 236,205,98); /* 256 bg */
	my_init_color(28, 249,247,243); /* 512 fg */
	my_init_color(29, 237,200,80); /* 512 bg */
	my_init_color(30, 249,247,243); /* 1024 fg */
	my_init_color(31, 236,196,63); /* 1024 bg */
	my_init_color(32, 249,247,243); /* 2048 fg */
	my_init_color(33, 236,195,45); /* 2048 bg */

	init_pair(1, 10, 10); /* border */
	init_pair(2, 11, 11); /* bg */
	init_pair(3, 12, 13); /* 2 */
	init_pair(4, 14, 15); /* 4 */
	init_pair(5, 16, 17); /* 8 */
	init_pair(6, 18, 19); /* 16 */
	init_pair(7, 20, 21); /* 32 */
	init_pair(8, 22, 23); /* 64 */
	init_pair(9, 24, 25); /* 128 */
	init_pair(10, 26, 27); /* 256 */
	init_pair(11, 28, 29); /* 512 */
	init_pair(12, 30, 31); /* 1024 */
	init_pair(13, 32, 33); /* 2048 */
	init_pair(14, COLOR_BLACK, COLOR_CYAN); /* >= 4096 */
}

void	playing(t_board *board)
{
	while(1)
	{
		// TIMO's turn display GRID

		// retrieve key move and update GRID
		int	key = KEY_RIGHT;
		launch_arrows(board, key);
		// add new position to the board
		srand(time(NULL));
		ft_printf("\n");
		outputGrid(board);
		t_pos pos1 = getRandomZeroPos(board);
		outputPos("pos1: ", pos1);
		initPosition(board, pos1);
		outputGrid(board);
		setZeroAmount(board);
		if(board->zero_amount <= 0 && noMovePossible(board) == true)
		{
			ft_printf("GAME OVER\n");
			return ;
		}
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_board	*board;
	int		key;

	(void)argc;
	(void)argv;
	
	/* ncurses setup (compile with -lnursesw)*/
	ft_setenv(envp, "TERM", "xterm-256color");
	setlocale(LC_ALL, "");
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	cbreak();
	curs_set(0);
	start_color();

	init_colors();
	board = init_board();

	srand(time(NULL));
	t_pos pos1 = getRandomZeroPos(board);
	srand(time(NULL) / 3);
	t_pos pos2 = getRandomZeroPos(board);
	while(pos1.x == pos2.x && pos1.y == pos2.y)
		pos2 = getRandomZeroPos(board);
	initPosition(board, pos1);
	initPosition(board, pos2);
	// playing(board);

	print_board(board);
	while ((key = getch()))
	{
		if (key == KEY_RIGHT)
			right(board);
		else if (key == KEY_LEFT)
			left(board);
		else if (key == KEY_UP)
			right(board);
		else if (key == KEY_DOWN)
			left(board);
		else if (key == 'q')
			break ;
		else if (key == 27)
			break ;
		clear();
		print_board(board);
		refresh();
	}

	destroy_board(board);
	endwin();
	return (0);
}
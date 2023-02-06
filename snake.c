#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

const char message_name[] = "SNAKE!";
const char message_instruction[] = "Please, enter the button 'ENTER' on your computer or laptop";
const char second_part_message_instruction[] = "You should to collect 60 point for win. Good luck!";
const char win_message[] = "You win)";
const char lose_message[] = "You lose(";

const char head = '@';
const char body = 'o';
const char point = '*';


struct part_of_snake{
	int y;
	int x;
	struct part_of_snake *next;
};

enum {
	duration_step = 70,
	key_enter = 10,
	key_escape = 27
};

static void Has_color()
{
	if (has_colors() == FALSE) 
	{
		endwin();
		exit(1);
	}
}

static void Message_end_game(int row, int col, const char* end_message)
{
	clear();
	move(row/2, (col - sizeof(end_message) - 1) / 2);
	addstr(end_message);
	move(row/2 + 1, (col - sizeof(message_instruction) - 1) / 2);
	addstr(message_instruction);
	refresh();
	int key_end;
	while ((key_end = getch()) != key_enter) {}
}

static int choice_dificult_game(int row, int col)
{
	clear();
	int count = 0, key, main_res;
	char data[5][20] = {"Easy","Normal" ,"Hard", "Very Hard", "Window difficult"};
	move(row/2, (col - sizeof(data[count]) + 1) / 2);
	addstr(data[count]);
	while ((key = getch()) != key_enter)
	{
		switch(key)
		{
		case KEY_LEFT:
			clear();
			--count;
			if (count < 0)
				count = 4;
			move(row/2, (col - sizeof(data[count]) + 1)/2);
			addstr(data[count]);
			refresh();
			break;
		case KEY_RIGHT:
			clear();
			++count;
			count = count % 5;
			move(row/2, (col - sizeof(data[count]) + 1) / 2);
			addstr(data[count]);
			refresh();
			break;
		default:
			break;
		}
	};
	switch(count)
	{
		case 0: main_res = 30; break;
		case 1: main_res = 60; break;
		case 2: main_res = 90; break;
		case 3: main_res = 120; break;
		case 4: main_res = (row * col); break;
		default:
			main_res =  -1;
	};
	clear();
	return main_res;
}

static int is_off_snake_on_window(int row, int col, struct part_of_snake* res)
{
	if (res->x >= col || res->y >= row || res->x < 0 || res->y < 0)
		return 1;
	return 0;
}

static int is_crashs_snake(struct part_of_snake* res)
{
	struct part_of_snake* temp = res;
	int flag = 0;
	while (temp)
	{
		if (temp != res && temp->x == res->x && temp->y == res->y)
		{
			flag = 1;
			return flag;
		}
		temp = temp->next;
	}
	return flag;
}

static void start_menu(int row, int col)
{
	move(row/2, (col - sizeof(message_name) - 1) / 2);
	addstr(message_name);
	move(row/2 + 1, (col - sizeof(message_instruction) - 1) / 2);
	addstr(message_instruction);
	move(row/2 + 2, (col - sizeof(second_part_message_instruction) - 1) / 2);
	addstr(second_part_message_instruction);
	refresh();
	int key_for_start_game;
	while ((key_for_start_game = getch()) != key_enter) {}
	clear();
	refresh();
}

static struct part_of_snake* add_part_of_snake(int* count, int dy, int dx, struct part_of_snake* snk)
{
	int dstrx = dx, dstry = dy; 
	*count = 0;
	struct part_of_snake *res = snk;
	while (res->next) 
	{
		dstry = res->next->y - res->y;
		dstrx = res->next->x - res->x;
		res = res->next;
		(*count)++;
	}
	struct part_of_snake *temp = (struct part_of_snake*) malloc(sizeof(struct part_of_snake));
	temp->next = NULL;
	(*count)++;
	temp->x = (res->x) + dstrx;
	temp->y = (res->y) + dstry;
	res->next = temp;
	move(temp->y, temp->x);
	addch(body);
	refresh();
	return snk;
}

static void random_point(int* y, int* x, int row, int col, struct part_of_snake* res)
{
	int flag_not_in_snake_a_point = 0;
	while (!flag_not_in_snake_a_point)
	{
		*y = rand() % (row - 6) + 3;
		*x = rand() % (col - 6) + 3;
		while (res)
		{
			if ((*y == (res->y)) && (*x == (res->x)))
			{
				flag_not_in_snake_a_point *= 0;
				break;
			}
			else
				++flag_not_in_snake_a_point;

			res = res->next;
		}
	}
}

static struct part_of_snake* traffic_of_snake(int dx, int dy, struct part_of_snake* res)
{
	int flag_body = 1, tempx = 0, tempy = 0, star_x, star_y;
	struct part_of_snake* temp = res;
	while (temp)
	{	
		move(temp->y, temp->x);
		addch(' ');
		if (!flag_body)
		{
			star_x = temp->x;
			star_y = temp->y;
			temp->x = tempx;
			temp->y = tempy;
			tempx = star_x;
			tempy = star_y;
		}
		else 
		{
			tempx = temp->x;
			tempy = temp->y;
			temp->x += dx;
			temp->y += dy;
		}
		move(temp->y, temp->x);
		if (res == temp)
		{
			addch(head);
		}
		else
			addch(body);
		temp = temp->next;
		flag_body = 0;
	}
	return res;
}


int main()
{
	int row, col, XPoint , YPoint, key, dy = 0, dx = 0, main_res;
	initscr();
	Has_color();
	getmaxyx(stdscr, row, col);
	curs_set(0);
	keypad(stdscr, 1);
	srand(time(NULL));
	timeout(duration_step);
	start_menu(row, col);
	main_res = choice_dificult_game(row, col);
	if (main_res == -1)
		endwin();
	struct part_of_snake *snake = (struct part_of_snake*) malloc(sizeof(struct part_of_snake));
	snake->x = col/2 + 1;
	snake->y = row/2 + 1;
	snake->next = NULL;
	move(snake->y, snake->x);
	addch(head);
	random_point(&YPoint, &XPoint, row, col, snake); 
	move(YPoint, XPoint);
	addch(point);
	int lose_flag = 0, count;
	while ((key = getch()) != key_escape)
	{
		switch(key)
		{
		case KEY_UP:
			if (dy == 1 && dx == 0)
				break;
			dy = -1; dx = 0;
			break;
		case KEY_DOWN:
			if (dy == -1 && dx == 0)
				break;
			dy = 1; dx = 0;
			break;
		case KEY_LEFT:
			if (dy == 0 && dx == 1)
				break;
			dy = 0; dx = -1;
			break;
		case KEY_RIGHT:
			if (dy == 0 && dx == -1)
				break;
			dy = 0; dx = 1;
			break;
		}
		snake = traffic_of_snake(dx, dy, snake);
		lose_flag = is_off_snake_on_window(row, col, snake) + is_crashs_snake(snake);
		if (lose_flag)
		{
			Message_end_game(row, col, lose_message);
			break;
		}
		if ((snake->x == XPoint) && (snake->y == YPoint))
		{
			snake = add_part_of_snake(&count,dy, dx, snake);
			random_point(&YPoint, &XPoint, row, col, snake);
			move(YPoint, XPoint);
			addch(point);
			mvprintw(0, 0, "%d", count);
		}
		if (count ==  main_res)
		{
			Message_end_game(row, col, win_message);
			break;
		}
	}	
	endwin();
	return 0;
}

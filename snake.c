#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#define CPS CLOCKS_PER_SEC
#define MAX_LENGTH 500
#define SNAKE_BODY '&'
#define BONUS '*'
#define START_LENGTH 3
// #define print(x, y, str) do{ move(y, x); printw(str); } while(0);


enum { key_escape = 27, key_pause = 32, key_new_game = 10 };
// enum { delay_duration = 200 };

typedef struct snake {
	int x, y;
} snake;

typedef struct boundary {
	int min_x, max_x, min_y, max_y;
	int is_failed;
} boundary;
static int score;
static int std_color = 0;
static int snake_color = 1;
static int bonus_color = 2;

void game_field(boundary *, int , int );	// create new game gield
void show_head(int , int );
void check_bonus(snake *, snake *, int *);
void create_bonus(snake *);
bool boundary_collision(boundary *, int , int );
bool body_collision(snake *, int *, int , int );
void delete_snake(snake *, int *);
void stop_snake(int *, int *, int *);
void info();

void init(snake *s, snake *bonus, boundary *coord, int *length) {
	int row, col;
	getmaxyx(stdscr, row, col);
	score = 0;
	coord->is_failed = 0;
	game_field(coord, row, col);
	
	int x = col / 2;
	int y = row / 2;
	
	*length = START_LENGTH;
	int i;
	for(i = 0; i < *length; i++) {
		s[i].x = x - i;
		s[i].y = y;
		
		show_head(s[i].x, s[i].y);	
		
	}
	
	create_bonus(bonus);
}

void hide_tail(snake *s, int *length) {
	move(s[*length - 1].y, s[*length - 1].x);
	addch(' ');
	refresh();
}

void show_head(int x, int y) {
	attr_set(A_BOLD, snake_color, NULL);
	move(y, x);
	addch(SNAKE_BODY);
	refresh();
	
	attr_set(A_NORMAL, std_color, NULL);
	refresh();
}

void move_snake(snake *s, int *length, snake *bonus, boundary *coord, int *dx, int *dy) {

	int x = s->x + *dx;
	int y = s->y + *dy;
	
	if(boundary_collision(coord, x, y) || 
		body_collision(s, length, x, y)) 
	{
		stop_snake(&(coord->is_failed), dx, dy);
		move(7, 12);
		printw("Game Over");
	}
	
	if(*dx != 0 || *dy != 0) {	
		show_head(x, y);
		hide_tail(s, length);

		memmove(s + 1, s, (*length) * (sizeof(s[0])));
	
		s->x = x;
		s->y = y;
		
		check_bonus(s, bonus, length);
		
	}
}

void check_bonus(snake *s, snake *bonus, int *length) {
	if(s->x == bonus->x && s->y == bonus->y) {
		(*length)++;
		score++;
		// battr_set(A_NORMAL, std_color, NULL);
		move(1, 0);
		printw("Score: %d", score);
		
		create_bonus(bonus);
	}
}

bool boundary_collision(boundary *coord, int x, int y) {
	
	return (x == coord->min_x ||		// left
			y == coord->min_y ||		// up
			x == coord->max_x ||		// right
			y == coord->max_y);			// down
}

bool body_collision(snake *s, int *length, int x, int y) {
	int i;
	
	for(i = 1; i < *length; i++) {
		if(x == s[i].x && y == s[i].y) {
			return true;
		}
		i++;
	}
	return false;
}

void stop_snake(int *var, int *dx, int *dy) { 
	*var = 1;
	*dx = 0;
	*dy = 0;
}

void delete_snake(snake *s, int *length) {
	memset(s, 0, (*length) * sizeof(s[0]));
}

void game_field(boundary *coord, int row, int col) {
	int i, j;
	
	coord->min_x = (col - 2 * row) - 1;	// left
	coord->min_y = 0;					// up
	coord->max_x = col - 1;				// right
	coord->max_y = row - 1;				// down
	
	// attr_set(A_NORMAL, std_color, NULL);
	
	for(i = 0; i < row; i++) {
		for(j = coord->min_x + 1; j < col; j++) {
			move(i, coord->min_x);
			printw("#");
			if(i == coord->min_y || i == coord->max_y) {
				move(i, j);
				printw("#");
			} else {
				move(i, j);
				printw(" ");
			}
			move(i, coord->max_x);
			printw("#");
		}
	}
	move(7, 12);
	printw("         ");	// 9 spaces to hide "Game Over"
	info();
	// refresh();
}

void info() {
	move(0, 13);
	printw("Snake");

	move(1, 0);
	printw("Score: %d", score);

	move(15, 0);
	printw("To start a game press any");
	move(16, 0);
	printw("arrow key");
	
	move(18, 0);
	printw("To pause a game press <Space>");
	
	move(20, 0);
	printw("To start a new game press");
	move(21, 0);
	printw("<Enter>");
}

void create_bonus(snake *bonus) {
	int x, y;
	y = rand() % 22 + 1;
	x = rand() % 47 + 32;

	bonus->x = x;
	bonus->y = y;
	
	attr_set(A_BOLD, bonus_color, NULL);
	move(bonus->y, bonus->x);
	addch(BONUS);

	refresh();
	
	attr_set(A_NORMAL, std_color, NULL);
	refresh();
	
}



void pause(int *dx, int *dy, int *save_x, int *save_y, int *is_paused) {
	if(*is_paused) {
		*dx = *save_x;
		*dy = *save_y;
		*is_paused = 0;
		move(7, 14);
		printw("     ");
	} else {
		*save_x = *dx;
		*save_y = *dy;
		stop_snake(is_paused, dx, dy);
		move(7, 14);
		printw("Pause");
	}
}

int main() {
	int key;
	int dx = 0, dy = 0;
	int save_x, save_y;
	
	int is_paused; // key_pressed;
	
	boundary coord;
	snake s [MAX_LENGTH]; 
	snake bonus;
	int length;
		
	// srand(time(NULL));
	
	int run = 1;
	clock_t movt;
	
	initscr();
	start_color();
	
	nodelay(stdscr, 1);
	keypad(stdscr, 1);
	noecho();
	curs_set(0);

	init_pair(snake_color, COLOR_GREEN, COLOR_BLACK);
	init_pair(bonus_color, COLOR_RED, COLOR_BLACK);
	// 


	init(s, &bonus, &coord, &length);

	is_paused = 0;
	//key_pressed = 0;
	movt = clock();
	
	while(run) {
		// if(key_pressed == 0)
		key = getch();
		
		if(coord.is_failed == 0) {
			if(is_paused == 0) { //  && key_pressed == 0
				switch(key) {
					/*case key_escape:
						run = 0;
						break;	
						*/				
					case KEY_UP:						
						dx = 0, 
						dy = -1;						
						break;
					case KEY_RIGHT:						
						dx = 1;
						dy = 0;						
						break;
					case KEY_DOWN:						
						dx = 0;
						dy = 1;									
						break;
					case KEY_LEFT:					
						dx = -1;
						dy = 0;				
						break;
					
				}
			}
			
			if(clock() - movt >= CPS * 0.2) {
				move_snake(s, &length, &bonus, &coord, &dx, &dy);
				movt += CPS * 0.2;
				// key_pressed = 0;
			}
		}
		
		switch(key) {
			case key_pause:
				if(coord.is_failed == 0)
					pause(&dx, &dy, &save_x, &save_y, &is_paused);
				break;
			case key_escape:
				run = 0;
				break;
				
			case key_new_game:
				stop_snake(&(coord.is_failed), &dx, &dy);
				is_paused = 0;

				delete_snake(s, &length);
				init(s, &bonus, &coord, &length);
				break;
				
		}	

	}
	endwin();
	
	return 0;
}

#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#define CPS CLOCKS_PER_SEC
#define MAX_LENGTH 500
#define SNAKE_BODY '&'
#define BONUS '*'

enum { key_escape = 27, key_pause = 32, key_new_game = 10 };
// enum { delay_duration = 200 };

typedef struct snake {
	int x, y;
} snake;

typedef struct boundary {
	int min_x, max_x, min_y, max_y;
	int is_failed;
} boundary;
static int score = 0;


bool check_connection(snake *, snake *);
void random_star(snake *);
void collision_check(snake *, boundary *, int *, int *, int *);
// void delete_snake(snake *s);
void stop_snake(int *, int *, int *);
/*
void init(snake *head, snake *n, int *row, int *col) {
		snake *tmp = malloc(sizeof(snake));
		tmp->cur_x = *col / 2;
		tmp->cur_y = *row / 2;
		tmp->next = NULL;
		tmp->prev = NULL;
		head = tmp;
		
		snake *new_el = malloc(sizeof(snake));
		n = new_el;
		random_star(n);
}
*/
/*
void connect(snake *head, snake *new_el) {	// snake **tail
	snake *tmp = malloc(sizeof(snake));
	tmp->cur_x = new_el->cur_x;
	tmp->cur_y = new_el->cur_y;
	
	tmp->next = head->next;
	tmp->prev = head;
	if(tmp->next)
		tmp->next->prev = tmp;
	
	head->next = tmp;
	
	score += 1;
	move(2, 0);
	printw("Your score: %d", score);
	//n++;
	
	// return head;
}
*/
void hide_tail(snake *s, int *length) {
	
		move(s[*length - 1].y, s[*length - 1].x);
		addch(' ');
	refresh();
}

void show_head(int x, int y) {
		move(y, x);
		addch(SNAKE_BODY);
	refresh();
	
}
/*
void step(snake *s) {
	if (s != NULL) {
		step(s->next);
		
		if(s->prev != NULL) {
			s->cur_x = s->prev->cur_x;
			s->cur_y = s->prev->cur_y;
		}
	}	
}
*/

void move_snake(snake *s, int *length, snake *bonus, boundary *coord, int *dx, int *dy) {

	collision_check(s, coord, length, dx, dy);
	/*
	int i;
	move(5, 0);
	printw("l=%d, score=%d", *length, score);
	for(i = 0; i < *length; i++) {
		move(7 + i, 0);
		printw("x=%d; y=%d", s[i].x, s[i].y);
	}
	// int grow = 0;
	*/
	if(*dx != 0 || *dy != 0) {
	
	int x = s->x + *dx;
	int y = s->y + *dy;
	
	
	show_head(x, y);
	
	hide_tail(s, length);

	memmove(s + 1, s, (*length) * (sizeof(s[0])));

	
	s->x = x;
	s->y = y;
	
	
	if(check_connection(s, bonus)) {
		(*length)++;
		random_star(bonus);
	}

	// score++;
	
	
	}
	
	
}
/*
void set_direction(snake *s, int dx, int dy) {
	if((s->dx == 0 && s->dy == 0) || (s->dx != 0 && dx == 0) || 
		(s->dy != 0 && dy == 0)) {
		s->dx = dx;
		s->dy = dy;
	}
}
*/
bool check_connection(snake *s, snake *bonus) {
	return (s->x == bonus->x && s->y == bonus->y);
}

void collision_check(snake *s, boundary *coord, int *length, int *dx, int *dy) {
	int x = s->x + *dx;
	int y = s->y + *dy;
	//move(0, 0);
	//printw("s->dx=%d, s->dy=%d", s->dx, s->dy);
	
	
	// collision with game-board boundaries
	if(x == coord->min_x ||		// left
		y == coord->min_y ||		// up
		x == coord->max_x ||		// right
		y == coord->max_y)			// down
	{
		stop_snake(&(coord->is_failed), dx, dy);
	}	
	// collison with snake`s body
	int i;
	
	for(i = 1; i < *length; i++) {
		//move(i, 0);
		//printw("n_x=%d,c_x=%d,n_y=%d,c_y=%d", x, s->cur_x, y, s->cur_y);
		if(x == s[i].x && y == s[i].y) {
			stop_snake(&(coord->is_failed), dx, dy);
			//move(0, 0);
			//printw("snake collision");
			break;
		}
		i++;

	}
}

void stop_snake(int *var, int *dx, int *dy) { 
	*var = 1;
	*dx = 0;
	*dy = 0;
}

/*
void delete_1(snake *s) {
	snake *tmp = malloc(sizeof(snake));
	tmp = s;
	while(tmp->next) {
		tmp = tmp->next;
	}
	free(tmp);
}
*/
/*
void delete_snake(snake *s) {
	if(s) {
		delete_snake(s->next);
		free(s);
	}
}
*/
void game_board(boundary *coord, int *row, int *col) {
	int i, j;
	
	coord->min_x = (*col - 2 * *row) - 1;	// left
	coord->min_y = 0;						// up
	coord->max_x = *col - 1;				// right
	coord->max_y = *row - 1;				// down
	
	for(i = 0; i < *row; i++) {
		for(j = coord->min_x + 1; j < *col; j++) {
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
}

void random_star(snake *bonus) {
	int x, y;
	y = rand() % 22 + 1;
	x = rand() % 47 + 32;

	
	bonus->x = x;
	bonus->y = y;
	
	move(bonus->y, bonus->x);
	addch(BONUS);

	// refresh();
	
	
}

void pause(int *dx, int *dy, int *save_x, int *save_y, int *is_paused) {
	if(*is_paused) {
		*dx = *save_x;
		*dy = *save_y;
		*is_paused = 0;
	} else {
		*save_x = *dx;
		*save_y = *dy;
		// move(0, 0);
		// printw("dx=%d; dy=%d", *dx, *dy);
		stop_snake(is_paused, dx, dy);
		
	}
}

int main() {
	int key, row, col;
	int dx = 0, dy = 0;
	int save_x, save_y;
	
	int is_paused, key_pressed;
	
	boundary coord;
	snake s [MAX_LENGTH], bonus;
	int length = 3;
	int i;
	
	// srand(time(NULL));
	
	int run = 1;
	clock_t movt;
	
	initscr();

	nodelay(stdscr, 1);
	keypad(stdscr, 1);
	noecho();
	curs_set(0);

	getmaxyx(stdscr, row, col);
	
	coord.is_failed = 0;
	game_board(&coord, &row, &col);

	
	//snake head;
	//snake n;

	int x = col / 2;
	int y = row / 2;

	random_star(&bonus);
	
	for(i = 0; i < length; i++) {
		s[i].x = x - i;
		s[i].y = y;
		
		show_head(s[i].x, s[i].y);
		

		
	}
	// set_direction(&head, 0, 0);
	
	is_paused = 0;
	//key_pressed = 0;
	movt = clock();
	
	while(run) {
		// if(key_pressed == 0)
		key = getch();
		
		//if(coord.is_failed == 0) {
			if(is_paused == 0) { //  && key_pressed == 0
				switch(key) {
					case key_escape:
						run = 0;
						break;					
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
		//}
		
		switch(key) {
			case key_pause:
				pause(&dx, &dy, &save_x, &save_y, &is_paused);
				break;
			case key_escape:
				run = 0;
				break;
				/*
			case key_new_game:
				coord.is_failed = 0;
				is_paused = 0;
				score = 0;
				game_board(&coord, &row, &col);
					
				delete_snake(head.next);
					
				head.cur_x = col / 2;
				head.cur_y = row / 2;
				head.next = NULL;
				head.prev = NULL;
					
				random_star(&n);

				show_snake(&head);
				set_direction(&head, 0, 0);
				break;
				*/
		}	

	}
	endwin();
	
	return 0;
}

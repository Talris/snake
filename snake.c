#include <curses.h>
#include <time.h>
#include <stdlib.h>


#define CPS CLOCKS_PER_SEC
#define SNAKE_BODY '&'
#define BONUS '*'

enum { key_escape = 27, key_pause = 32, key_new_game = 10 };
enum { delay_duration = 200 };

typedef struct snake {
	int cur_x, cur_y, dx, dy;
	struct snake *next, *prev;
} snake;

typedef struct boundary {
	int min_x, max_x, min_y, max_y;
	int is_failed;
} boundary;
static int score = 0;

bool check_connection(snake *s, snake *n);
void random_star(snake *n);
void collision_check(snake *s, boundary *coord, int x, int y);
void delete_snake(snake *s);
void stop_snake(snake *s, int *var);
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

void hide_snake(const snake *s) {
	while(s != NULL) {
	// move(tmp->cur_y, tmp->cur_x);
		move(s->cur_y, s->cur_x);
		addch(' ');
		s = s->next;
	}
	refresh();
}

void show_snake(const snake *s) {
	//snake *tmp = s;
	int n = 3;
	while(s != NULL) {
	
		move(s->cur_y, s->cur_x);
		addch(SNAKE_BODY);
		/*
		move(n, 0);
		printw("move(%d, %d)", s->cur_y, s->cur_x);
		n++;
		*/
		s = s->next;
		
	}
	//move(1, 0);
	//printw("%d", n);
	refresh();
	
}

void step(snake *s) {
	if (s != NULL) {
		step(s->next);
		
		if(s->prev != NULL) {
			s->cur_x = s->prev->cur_x;
			s->cur_y = s->prev->cur_y;
		}
	}	
}


void move_snake(snake *s, snake *n, boundary *coord) {
	int x = s->cur_x + s->dx;
	int y = s->cur_y + s->dy;
	collision_check(s, coord, x, y);
	
	hide_snake(s);
	// 
	if((s->dx != 0 || s->dy != 0) && s->next &&
		// after eating n, head and n is equal, so need to move only head
		(s->cur_x != s->next->cur_x || s->cur_y != s->next->cur_y)) 
	{
		step(s);

	}
	// new coord of head
	s->cur_x = x;		
	s->cur_y = y;
	
	if(check_connection(s, n)) {
		connect(s, n);
		random_star(n);
	}
	
	show_snake(s);
}

void set_direction(snake *s, int dx, int dy) {
	if((s->dx == 0 && s->dy == 0) || (s->dx != 0 && dx == 0) || 
		(s->dy != 0 && dy == 0)) {
		s->dx = dx;
		s->dy = dy;
	}
}

bool check_connection(snake *s, snake *n) {
	return (s->cur_x == n->cur_x && s->cur_y == n->cur_y);
}

void collision_check(snake *s, boundary *coord, int x, int y) {
	
	move(0, 0);
	printw("s->dx=%d, s->dy=%d", s->dx, s->dy);
	
	
	// collision with game-board boundaries
	if(x == coord->min_x ||		// left
		y == coord->min_y ||		// up
		x == coord->max_x ||		// right
		y == coord->max_y)			// down
	{
		stop_snake(s, &(coord->is_failed));
	}	
	// collison with snake`s body
	int i = 3;
	
	for(s = s->next; s != NULL; s = s->next) {
		move(i, 0);
		printw("n_x=%d,c_x=%d,n_y=%d,c_y=%d", x, s->cur_x, y, s->cur_y);
		if(x == s->cur_x && y == s->cur_y) {
			stop_snake(s, &(coord->is_failed));
			//move(0, 0);
			//printw("snake collision");
			break;
		}
		i++;

	}
}

void stop_snake(snake *s, int *var) { 
	*var = 1;
	set_direction(s, 0, 0);
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

void delete_snake(snake *s) {
	if(s) {
		delete_snake(s->next);
		free(s);
	}
}

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

void random_star(snake *n) {
	int x, y;
	y = rand() % 22 + 1;
	x = rand() % 47 + 32;
	// snake *tmp = malloc(sizeof(snake));
	
	n->cur_x = x;
	n->cur_y = y;
	n->next = NULL;
	
	move(n->cur_y, n->cur_x);
	addch(BONUS);

	refresh();
	
	// return tmp;
}

void pause(snake *s, int *dx, int *dy, int *is_paused) {
	if(*is_paused) {
		set_direction(s, *dx, *dy);
		*is_paused = 0;
	} else {
		*dx = s->dx;
		*dy = s->dy;
		// move(0, 0);
		// printw("dx=%d; dy=%d", *dx, *dy);
		stop_snake(s, is_paused);
		
	}
}

int main() {
	int key, row, col;
	int dx, dy;
	int is_paused, key_pressed;
	boundary coord;
	snake head, n;
	

	int run = 1;
	clock_t movt;
	
	initscr();
	// cbreak();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
	// timeout(delay_duration);
	// halfdelay(2);
	getmaxyx(stdscr, row, col);
	
	coord.is_failed = 0;
	game_board(&coord, &row, &col);

	
	//snake head;
	//snake n;

	head.cur_x = col / 2;
	head.cur_y = row / 2;
	head.next = NULL;
	head.prev = NULL;
	
	random_star(&n);

	show_snake(&head);
	set_direction(&head, 0, 0);
	
	is_paused = 0;
	key_pressed = 0;
	movt = clock();
	
	while(run) {
		// if(key_pressed == 0)
		key = getch();
		
		if(coord.is_failed == 0) {
			if(is_paused == 0) { //  && key_pressed == 0
				switch(key) {
					case key_escape:
						run = 0;
						break;					
					case KEY_UP:						
						set_direction(&head, 0, -1);
						key_pressed = 1;
						break;
					case KEY_RIGHT:						
						set_direction(&head, 1, 0);
						key_pressed = 1;				
						break;
					case KEY_DOWN:						
						set_direction(&head, 0, 1);
						key_pressed = 1;			
						break;
					case KEY_LEFT:					
						set_direction(&head, -1, 0);	
						key_pressed = 1;			
						break;
					
				}
			}
			
			if(clock() - movt >= CPS * 0.2) {
				move_snake(&head, &n, &coord);
				movt += CPS * 0.2;
				// key_pressed = 0;
			}
		}
		switch(key) {
			case key_pause:
				pause(&head, &dx, &dy, &is_paused);
				break;
			case key_escape:
				run = 0;
				break;
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
		}	

	}
	endwin();
	
	return 0;
}

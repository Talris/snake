#include <curses.h>
#include <stdlib.h>

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

bool check_connection(snake *s, snake *n);
void random_star(snake *n);
void collision_check(snake *s, boundary *coord);
void delete_snake(snake *s);
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
	
	static int n = 1;
	move(2, 0);
	printw("added:%d", n);
	n++;
	
	// return head;
}

void hide_star(const snake *s) {
	while(s != NULL) {
	// move(tmp->cur_y, tmp->cur_x);
		move(s->cur_y, s->cur_x);
		addch(' ');
		s = s->next;
	}
	refresh();
}

void show_star(const snake *s) {
	//snake *tmp = s;
	int n = 3;
	while(s != NULL) {
	
		move(s->cur_y, s->cur_x);
		addch('*');
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
	collision_check(s, coord);
	
	hide_star(s);
	
	if((s->dx != 0 || s->dy != 0) && s->next && 
		(s->cur_x != s->next->cur_x || s->cur_y != s->next->cur_y)) 
	{
		step(s);

	}
	// new coord of head
	s->cur_x += s-> dx;		
	s->cur_y += s-> dy;
	
	if(check_connection(s, n)) {
		connect(s, n);
		random_star(n);
	}
	
	show_star(s);
}

void set_direction(snake *s, int dx, int dy) {
	s->dx = dx;
	s->dy = dy;
}

bool check_connection(snake *s, snake *n) {
	return (s->cur_x == n->cur_x && s->cur_y == n->cur_y);
}

void collision_check(snake *s, boundary *coord) {
	int next_x, next_y;
	
	next_x = s->cur_x + s->dx;
	next_y = s->cur_y + s->dy;
	
	// collision with game-board boundaries
	if(next_x == coord->min_x ||		// left
		next_y == coord->min_y ||		// up
		next_x == coord->max_x ||		// right
		next_y == coord->max_y)			// down
	{
		coord->is_failed = 1;
		set_direction(s, 0, 0);		
	}
	
	s = s->next;
	// collison with snake`s body
	while(s) {
		if(next_x == s->cur_x && next_y == s->cur_y) {
			coord->is_failed = 1;
			set_direction(s, 0, 0);
			break;
		}
		s = s->next;
	}
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
	coord->min_x = (*col - 2 * *row) - 1;	// left
	coord->min_y = 0;						// up
	coord->max_x = *col - 1;				// right
	coord->max_y = *row - 1;				// down
	
	int i, j;
	
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
	addch('*');

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
		set_direction(s, 0, 0);
		*is_paused = 1;
	}
}

int main() {
	int key, row, col;
	int dx, dy;
	int is_paused;
	boundary coord;
	snake head, n;
	
	initscr();
	cbreak();
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
	timeout(delay_duration);
	
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

	show_star(&head);
	set_direction(&head, 0, 0);
	
	is_paused = 0;
	
	while((key = getch()) != key_escape) {
		if(coord.is_failed == 0) {
			switch(key) {
				case key_pause:
					pause(&head, &dx, &dy, &is_paused);
					break;
				case KEY_UP:
					set_direction(&head, 0, -1);
					break;
				case KEY_RIGHT:
					set_direction(&head, 1, 0);
					break;
				case KEY_DOWN:
					set_direction(&head, 0, 1);
					break;
				case KEY_LEFT:
					set_direction(&head, -1, 0);

					break;
				case ERR:
					move_snake(&head, &n, &coord);
					break;
			} 
		} else {
			switch(key) {
				case key_new_game:
					coord.is_failed = 0;
					game_board(&coord, &row, &col);
					
					delete_snake(head.next);
					
					head.cur_x = col / 2;
					head.cur_y = row / 2;
					head.next = NULL;
					head.prev = NULL;
					
					random_star(&n);

					show_star(&head);
					set_direction(&head, 0, 0);
					break;
			}	
		}		
	}
	endwin();
	
	return 0;
}

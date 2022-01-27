#include <curses.h>
#include <stdlib.h>

enum { key_escape = 27, key_pause = 32 };
enum { delay_duration = 200 };

typedef struct snake {
	int cur_x, cur_y, dx, dy;
	struct snake *next, *prev;
} snake;

int check(snake *s, snake *n);
void random_star(snake *n);

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

void increase_snake(snake *head, snake *new_el) {	// snake **tail
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
	// int x, y;
	// snake **tmp = s;
	if (s != NULL) {
		//return NULL;
	
		step(s->next);
		if(s->prev != NULL) {
		//return s;
		
			s->cur_x = s->prev->cur_x;
			s->cur_y = s->prev->cur_y;
		}
	
	}
	//return s;
	
}


void move_snake(snake *s, snake *n) {
	int is_encountered;
	hide_star(s);
	
	if((s->dx != 0 || s->dy != 0) && s->next && 
		(s->cur_x != s->next->cur_x || s->cur_y != s->next->cur_y)) 
	{
		step(s);

	}
	s->cur_x += s-> dx;		// new coord of head
	s->cur_y += s-> dy;
	
	is_encountered = check(s, n);
	if(is_encountered)
		random_star(n);
	
	
	show_star(s);
}

void set_direction(snake *s, int dx, int dy) {
	s->dx = dx;
	s->dy = dy;
}

int check(snake *s, snake *n) {
	int encounter = 0;
	if(s->cur_x == n->cur_x && s->cur_y == n->cur_y) {

		increase_snake(s, n);

		
		encounter = 1;
		// move(0, 0);
		// printw("connected");
	}
	
	return encounter;
}

void game_board(int *row, int *col) {
	int left = (*col - 2 * *row) - 1;
	// int right = left + 2 * row - 1;
	int i;
	
	for(i = 0; i < *row; i++) {
		move(i, left);
		printw("#");
		// move(i, right);
		//  printw("#");
	}
}

void random_star(snake *n) {
	int x, y;
	y = rand() % 24;
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
		move(0, 0);
		printw("dx=%d; dy=%d", *dx, *dy);
		set_direction(s, 0, 0);
		*is_paused = 1;
	}
}

int main() {
	int key, row, col;
	int dx, dy;
	int is_paused;
	
	initscr();
	cbreak();
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
	timeout(delay_duration);
	
	getmaxyx(stdscr, row, col);
	
	game_board(&row, &col);

	
	snake head;
	snake n;

	head.cur_x = col / 2;
	head.cur_y = row / 2;
	head.next = NULL;
	head.prev = NULL;

	random_star(&n);

	
	show_star(&head);
	set_direction(&head, 0, 0);
	
	is_paused = 0;
	
	while((key = getch()) != key_escape) {
		switch(key) {
			case key_pause:
				pause(&head, &dx, &dy, &is_paused);
				break;
			case KEY_UP:
				set_direction(&head, 0, -1);
				//move_snake(&head, &n);
				break;
			case KEY_RIGHT:
				set_direction(&head, 1, 0);
				//move_snake(&head, &n);
				break;
			case KEY_DOWN:
				set_direction(&head, 0, 1);
				//move_snake(&head, &n);
				break;
			case KEY_LEFT:
				set_direction(&head, -1, 0);
				//move_snake(&head, &n);
				break;
			case ERR:
				move_snake(&head, &n);
				break;
		}
		
	}
	endwin();
	
	return 0;
}

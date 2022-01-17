#include <curses.h>
#include <stdlib.h>

enum { key_escape = 27, key_pause = 32 };

typedef struct snake {
	int cur_x, cur_y, dx, dy;
	struct snake *next;
} snake;

snake *check(snake *s, snake *n);





snake *increase_snake(snake *head, snake *new_el) {	// stack // snake **tail
	// snake *tmp = malloc(sizeof(snake));
	new_el->next = head->next;
	head->next = new_el;
	
	return head;
}

void hide_star(snake *s) {
	while(s != NULL) {
	// move(tmp->cur_y, tmp->cur_x);
		move(s->cur_y, s->cur_x);
		addch(' ');
		s = s->next;
	}
	refresh();
}

void show_star(snake *s) {
	//snake *tmp = s;
	int n = 0;
	while(s != NULL) {
	
		move(s->cur_y, s->cur_x);
		addch('*');
		s = s->next;
		n++;
	}
	move(1, 0);
	printw("%d", n);
	refresh();
	
}

snake *one_step(snake *s) {
	// int x, y;
	snake *tmp = s;
	while(tmp->next != NULL) {
		tmp->next->cur_x = tmp->cur_x;
		tmp->next->cur_y = tmp->cur_y;
		tmp = tmp->next;
	}
	
	return s;
}


void move_snake(snake *s, snake *n) {
	hide_star(s);
	s = one_step(s);
	s->cur_x += s-> dx;		// new coord of head
	s->cur_y += s-> dy;
	
	
	s = check(s, n);
	
	
	
	show_star(s);
}

void set_direction(snake *s, int dx, int dy) {
	s->dx = dx;
	s->dy = dy;
}

snake *check(snake *s, snake *n) {
	if(s->cur_x == n->cur_x && s->cur_y == n->cur_y) {
		snake *tmp = malloc(sizeof(snake));
		// tmp = n;
		
		// tmp->cur_x += s-> dx;
		// tmp->cur_y += s-> dy;
		s = increase_snake(s, n);
		//hide_star(n);
		//free(n);
		
		move(0, 0);
		printw("connected");
	}
	
	return s;
}

void game_board(int row, int col) {
	int left = (col - 2 * row) - 1;
	// int right = left + 2 * row - 1;
	int i;
	
	for(i = 0; i < row; i++) {
		move(i, left);
		printw("#");
		// move(i, right);
		// printw("#");
	}
}

void pause(int is_paused) {
	//long time = 31536000000; // year
	
}
snake *random_star() {
	snake *tmp = malloc(sizeof(snake));
	tmp->cur_x = 45;
	tmp->cur_y = 12;
	tmp->next = NULL;
	
	move(tmp->cur_y, tmp->cur_x);
	addch('*');
	refresh();
	
	return tmp;
}
int main() {
	int key, row, col;// is_paused;
	
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, 1);

	getmaxyx(stdscr, row, col);
	
	//game_board(row, col);
	// snake s;
	
	snake *head = NULL;
	snake *n = NULL;
	//snake *tail = NULL;
	head = malloc(sizeof(snake));
	head->cur_x = col / 2;
	head->cur_y = row / 2;
	head->next = NULL;

	// is_paused = 0;
	
	//head = increase_snake(head, &s); 
	show_star(head);

	n = random_star();
	while((key = getch()) != key_escape) {
		switch(key) {
			// case key_pause:
			
			case KEY_UP:
				set_direction(head, 0, -1);
				move_snake(head, n);
				break;
			case KEY_RIGHT:
				set_direction(head, 1, 0);
				move_snake(head, n);
				break;
			case KEY_DOWN:
				set_direction(head, 0, 1);
				move_snake(head, n);
				break;
			case KEY_LEFT:
				set_direction(head, -1, 0);
				move_snake(head, n);
				break;
		}
		
	}
	endwin();
	
	return 0;
}

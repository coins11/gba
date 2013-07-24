#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/delay.h"
#include "lib/print.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLUE      BGR(0, 0, 31)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_GREEN     BGR(0, 31, 0)
#define COLOR_VIOLET    BGR(31, 10, 31)
#define COLOR_BLACK     0

#define BALL_ID   1
#define BLOCK_ID  2
#define RACKET_ID 3

enum state {
	START, RUN, DIE, CLEAR, RESTART
} STATE;

inline void
die_on_bottom (Shape *s, int m) {
	if ( s->on_bottom_border(s) ) {
		STATE = DIE;
	}
}

inline int
shrink_racket (Shape *r, Shape *s) {
	if (s->id == BLOCK_ID) {
		if ( !r->as.box.resize(r, -10, 0) ) {
			STATE = DIE;
		}
	}

	return 0;
}

inline int
accelerate_racket (Shape *ball, Shape *block) {
	if (block->id == BLOCK_ID) {
		ball->v.add_v( &(ball->v), 1, 1 );
	}

	return 0;
}

inline int
pierce_block (Shape *b1, Shape *b2) {
	return (b2->id == BLOCK_ID);
}


inline void
init_circles (Shape *c, int l, int r, int x, int y) {
	for (int i = 0; i < l; i++) {
		new_Circle(&c[i]);
		c[i].id    = BALL_ID;
		c[i].color = COLOR_WHITE;
		c[i].as.circle.set( &c[i], x + 20 * i, y, r );

		if (i == 0) {
			c[i].v.set_v(&(c[i].v), 1, 1);
		} else {
			c[i].v.set_v(&(c[i].v), -2, -2);
		}

		c[i].move_callback  = die_on_bottom;
		c[i].touch_callback = accelerate_racket;
	}
}

inline void
init_blocks (Shape *b, int l, int s, int x, int y, int w, int h) {
	for (int i = s; i < l; i++) {
		new_Box(&b[i]);
		b[i].id    = BLOCK_ID;
		b[i].color = COLOR_RED;
		b[i].as.box.set(&b[i], (x + (i - s) * x), y, w, h);
		b[i].v.set_v(&(b[i].v), (i % 5), (i % 3));
		b[i].v.set_a(&(b[i].v), 0, 0);
		b[i].breakable = 1;
		b[i].touch_callback = pierce_block;
	}
}

inline void
move_racket (Shape *r, int key) {
	if (! (key & KEY_L)) {
		r->v.set_a(&(r->v), r->v.ax - 2, r->v.ay);
	} else if (! (key & KEY_R)) {
		r->v.set_a(&(r->v), r->v.ax + 2, r->v.ay);
	} else {
		r->v.set_a(&(r->v), 0, r->v.ay);
	}

	if (! (key & KEY_LEFT)) {
		r->v.set_v(&(r->v), r->v.dx - 1, r->v.dy);
	} else if (! (key & KEY_RIGHT)) {
		r->v.set_v(&(r->v), r->v.dx + 1, r->v.dy);
	} else {
		r->v.set_v(&(r->v), 0, r->v.dy);
	}
}

inline void
init (Shape *c, Shape *b, Shape *r) {
	init_circles(c, 2, 5, 70, 70);
	init_blocks(b, 10, 0, 20, 10, 6, 6);
	init_blocks(b, 20, 10, 20, 30, 6, 6);

	chain_shapes(23, &c[0], &c[1], &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7], &b[8], &b[9], &b[10], &b[11], &b[12], &b[13], &b[14], &b[15], &b[16], &b[17], &b[18], &b[19], r );
}

int
main () {
	hword *fb = (hword*)VRAM;
	hword begin;
	Shape c[2];
	Shape b[20];
	Shape r;
	int i, key;

	STATE = START;
	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	delay_init();

	init(c, b, &r);

	new_Box(&r);
	r.id    = RACKET_ID;
	r.color = COLOR_WHITE;
	r.as.box.set(&r, 120, 150, 40, 5);
	r.v.set_v(&(r.v), 0, 0);
	r.v.set_a(&(r.v), 0, 0);
	r.v.reflectable  = 0;
	r.touch_callback = shrink_racket;

	wait_until_vblank();
	r.draw_all(&r);
	wait_while_vblank();
	
	while (1) {
		key   = gba_register(KEY_STATUS);
		begin = gba_register(TMR_COUNT0);

		switch (STATE) {
			case START:
				if (! (key & KEY_START)) {
					STATE = RUN;
				}
				break;
			case DIE:
				wait_until_vblank();
				draw_str("DEAD", 50, 10, COLOR_WHITE);
				delay(9000);
				draw_str("DEAD", 50, 10, COLOR_BLACK);
				wait_while_vblank();

				STATE = RESTART;
				break;
			case CLEAR:
				wait_until_vblank();
				draw_str("CLEAR", 50, 10, COLOR_WHITE);
				delay(9000);
				draw_str("CLEAR", 50, 10, COLOR_BLACK);

				STATE = RESTART;
				break;
			case RESTART:
				init(c, b, &r);
				r.as.box.set(&r, 120, 150, 40, 5);

				chain_shapes(23, &r, &c[0], &c[1], &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7], &b[8], &b[9], &b[10], &b[11], &b[12], &b[13], &b[14], &b[15], &b[16], &b[17], &b[18], &b[19] );

				wait_until_vblank();
				r.erase_all(&r);
				r.draw_all(&r);
				wait_while_vblank();

				STATE = START;
				break;
			case RUN:
				move_racket(&r, key);

				if (r.run(&r) == 3) {
					STATE = CLEAR;
				}

				wait_until(begin + 500);

				wait_until_vblank();
				r.redraw_all(&r);
				wait_while_vblank();
				break;
		}
	}
}

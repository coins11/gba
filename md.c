#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/delay.h"
#include "lib/print.h"
#include "lib/libgba/gba_systemcalls.h"
#include "lib/debug.h"
#include <string.h>

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

static inline void
die_on_bottom (Shape *s, int m)
{
	if ( s->on_bottom_border(s) ) {
		STATE = DIE;
	}
}

static inline int
shrink_racket (Shape *r, Shape *s)
{
	if (s->id == BLOCK_ID) {
		if ( !r->as.box.resize(r, -10, 0) ) {
			STATE = DIE;
		}
	}

	return 0;
}

static inline int
accelerate_racket (Shape *ball, Shape *block)
{
	if (block->id == BLOCK_ID) {
		ball->v.up_v( &(ball->v), 1, 1 );
	}

	return 0;
}

static inline int
pierce_block (Shape *b1, Shape *b2)
{
	return (b2->id == BLOCK_ID) || (b2->id == BALL_ID);
}

static inline void
init_circles (Shape *c, int r, int x, int y, hword t)
{
	new_Circle(c);
	c->id    = BALL_ID;
	c->color = COLOR_WHITE;
	c->as.circle.set( c, x, y, r );

	c->color = COLOR_WHITE;
	c->v.set_v(&(c->v), (DivMod(t, 2) ? 2 : -2), (DivMod(t, 3) ? 2 : -2));

	c->move_callback = die_on_bottom;
	//c->touch_callback = accelerate_racket;
	c->update_mn(c);
}

static inline void
init_blocks (Shape *b, int l, int s, int x, int y, int w, int h)
{
	for (int i = s; i < l; i++) {
		new_Box(&b[i]);
		b[i].id    = BLOCK_ID;
		b[i].color = COLOR_RED;
		b[i].as.box.set(&b[i], (x + (i - s) * 20), y, w, h);
		b[i].v.set_v(&(b[i].v), (DivMod(i, 2) ? 1 : -1), (DivMod(i, 2) ? -1 : 1));
		b[i].v.set_a(&(b[i].v), 0, 0);
		b[i].breakable = 1;
		b[i].touch_callback = pierce_block;
		b[i].update_mn(&b[i]);
	}
}

static inline void
move_racket (Shape *r, int key)
{
	if (! (key & KEY_L)) {
		r->v.up_v(&(r->v), -2, 0);
	} else if (! (key & KEY_R)) {
		r->v.up_v(&(r->v), 2,  0);
	} else {
		r->v.down_v(&(r->v), 2, 0);
	}

	if (! (key & KEY_LEFT)) {
		r->v.set_v(&(r->v), -2, 0);
	} else if (! (key & KEY_RIGHT)) {
		r->v.set_v(&(r->v), 2,  0);
	}
}

static inline void
move_block (Shape *b, int l, int key)
{
	int i;
	int s = !(key & KEY_SELECT);

	for (i = 0; i < l; i++) {
		b[i].v.dx = (!s ? 0 : (DivMod(i, 2) ? 1 : -1));
		b[i].v.dy = (!s ? 0 : (DivMod(i, 2) ? -1 : 1));
	}
}

static inline void
init (Shape *c, Shape *b, Shape *r, hword begin)
{
	init_circles(c, 4, DivMod(begin, 220) + 10, 70, begin);
	init_blocks(b, 10, 0, 25, 10, 5, 5);
	init_blocks(b, 20, 10, 25, 30, 5, 5);

	r->as.box.set(r, 100, 150, 40, 5);

	chain_shapes(22, &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7], &b[8], &b[9], &b[10], &b[11], &b[12], &b[13], &b[14], &b[15], &b[16], &b[17], &b[18], &b[19], &c[0], r );
}

int
main ()
{
	hword *fb = (hword*)VRAM;
	hword begin;
	Shape c;
	Shape b[20];
	Shape r;
	int i, key, particle;

	STATE = START;
	gba_register(LCD_CTRL)   = LCD_BG2EN | LCD_MODE3;
	delay_init();

	new_Box(&r);
	r.id    = RACKET_ID;
	r.color = COLOR_WHITE;
	r.v.set_v(&(r.v), 0, 0);
	r.v.set_a(&(r.v), 0, 0);
	r.v.reflectable  = 0;
	r.touch_callback = shrink_racket;

	
	while (1) {
		key   = gba_register(KEY_STATUS);
		begin = gba_register(TMR_COUNT0);

		switch (STATE) {
			case START:
				wait_until_vblank();
				draw_str("Press START", 75, 100, COLOR_WHITE);
				wait_while_vblank();

				if (! (key & KEY_START)) {
					wait_until_vblank();
					draw_str("Press START", 75, 100, COLOR_BLACK);
					wait_while_vblank();

					init(&c, b, &r, begin);

					wait_until_vblank();
					r.draw_all(&r);
					wait_while_vblank();

					STATE = RUN;
				}
				move_block(b, 20, key);

				break;
			case DIE:
				wait_until_vblank();
				draw_str("DEAD", 100, 100, COLOR_WHITE);
				wait_while_vblank();

				if (! (key & KEY_START)) {
					STATE = RESTART;
				} else {
					break;
				}

				wait_until_vblank();
				draw_str("DEAD", 100, 100, COLOR_BLACK);
				wait_while_vblank();
				break;
			case CLEAR:
				wait_until_vblank();
				draw_str("CLEAR", 100, 100, COLOR_WHITE);
				wait_while_vblank();

				if (! (key & KEY_START)) {
					STATE = RESTART;
				} else {
					break;
				}

				wait_until_vblank();
				draw_str("CLEAR", 100, 100, COLOR_BLACK);
				wait_while_vblank();
				break;
			case RESTART:
				wait_until_vblank();
				r.clear_all(&r);
				wait_while_vblank();

				delay(60000);

				STATE = START;
				break;
			case RUN:
				if (! (key & KEY_A) && ! (key & KEY_B)) {
					STATE = RESTART;
				} 
				move_racket(&r, key);

				particle = r.run(&r);
				if (particle <= 2) {
					STATE = CLEAR;
				}

				wait_until_vblank();
				r.redraw_all(&r);
				wait_while_vblank();

				wait_until(begin + 10000 + (particle - 2) * 500);
				break;
		}
	}
}

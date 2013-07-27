#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/debug.h"
#include "lib/print.h"
#include "lib/delay.h"
#include <string.h>

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLUE      BGR(0, 0, 31)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_GREEN     BGR(0, 31, 0)
#define COLOR_VIOLET    BGR(25, 10, 25)
#define COLOR_BLACK     0


inline void
move_racket (Shape *r, int key) {
	if (! (key & KEY_L)) {
		r->v.up_a(&(r->v), -2, 0);
	} else if (! (key & KEY_R)) {
		r->v.up_a(&(r->v), 2,  0);
	} else {
		r->v.down_a(&(r->v), 2, 0);
	}

	if (! (key & KEY_LEFT)) {
		r->v.up_v(&(r->v), -2, 0);
	} else if (! (key & KEY_RIGHT)) {
		r->v.up_v(&(r->v), 2,  0);
	} else {
		r->v.down_v(&(r->v), 2, 0);
	}
}


int
main () {
	hword *fb = (hword*)VRAM;
	hword begin;
	Shape c[3];
	Shape b[2];
	int key, i, y;
	Shape *s, *n;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	delay_init();

	new_Circle(&c[0]);
	c[0].id    = 0;
	c[0].color = COLOR_WHITE;
	c[0].as.circle.set( &c[0], 90, 50, 4 );
	c[0].v.set_v(&(c[0].v), 0, 0);
	c[0].v.set_a(&(c[0].v), 0, 0);

	new_Circle(&c[1]);
	c[1].id    = 1;
	c[1].color = COLOR_RED;
	c[1].as.circle.set( &c[1], 10, 10, 4 );
	c[1].v.set_v(&(c[1].v), 0, 0);
	c[1].v.set_a(&(c[1].v), 0, 0);

	new_Circle(&c[2]);
	c[2].id    = 2;
	c[2].color = COLOR_BLUE;
	c[2].as.circle.set( &c[2], 200, 120, 4 );
	c[2].v.set_v(&(c[2].v), 0, 0);
	c[2].v.set_a(&(c[2].v), 0, 0);

	new_Box(&b[0]);
	b[0].id    = 3;
	b[0].color = COLOR_VIOLET;
	b[0].as.box.set(&b[0], 30, 100, 5, 5);
	b[0].v.set_v(&(b[0].v), 0, 0);
	b[0].v.set_a(&(b[0].v), 0, 0);

	new_Box(&b[1]);
	b[1].id    = 4;
	b[1].color = COLOR_WHITE;
	b[1].as.box.set(&b[1], 100, 100, 5, 5);
	b[1].v.set_v(&(b[1].v), 0, 0);
	b[1].v.set_a(&(b[1].v), 0, 0);
	b[1].v.reflectable = 0;

	chain_shapes(5, &c[0], &c[1], &c[2], &b[0], &b[1]);
	
	c[0].update_mn_all(&c[0]);

	wait_until_vblank();
	c[0].draw_all(&c[0]);
	wait_while_vblank();

	while (1) {
		key = gba_register(KEY_STATUS);
		begin = gba_register(TMR_COUNT0);

		if (! (key & KEY_LEFT)) {
			b[1].v.up_a(&(b[1].v), -2, 0);
		}
		else if (! (key & KEY_RIGHT)) {
			b[1].v.up_a(&(b[1].v), 2, 0);
		}
		else {
			b[1].v.set_a(&(b[1].v), 0, b[1].v.ay);
			b[1].v.set_v(&(b[1].v), 0, b[1].v.dy);
		}

		if (! (key & KEY_DOWN)) {
			b[1].v.up_a(&(b[1].v), 0, 2);
		}
		else if (! (key & KEY_UP)) {
			b[1].v.up_a(&(b[1].v), 0, -2);
		}
		else {
			b[1].v.set_a(&(b[1].v), b[1].v.ax, 0);
			b[1].v.set_v(&(b[1].v), b[1].v.dx, 0);
		}

		c[0].run(&c[0]);

		wait_until_vblank();
		c[0].redraw_all(&c[0]);
		wait_while_vblank();

		wait_until_vblank();
		for (i = 0; i <= 120; i++) {
			memset((fb + 10 + i * LCD_WIDTH), COLOR_BLACK, 128*2);
		}

		//draw_int(b[1].mn, 10, 0, COLOR_WHITE);
		//draw_int(b[1].p.x, 10, 10, COLOR_WHITE);
		//draw_int(b[1].p.y, 10, 20, COLOR_WHITE);
		draw_int(c[0].mn, 10, 0, COLOR_WHITE);
		draw_int( b[1].same_space(&b[1], &c[0]), 10, 10, c[0].color );
		for (s = &c[1], i = 1; s != NULL && s != &c[0]; s = s->next, i+=2) {
			draw_int(s->mn, 10, 10 + 10 * i, s->color);
			draw_int( b[1].same_space(&b[1], s), 10, 10 + 10 * (i + 1), s->color );
		}

		wait_while_vblank();
	}
}

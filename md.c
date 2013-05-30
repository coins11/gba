#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/delay.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLUE      BGR(0, 0, 31)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_GREEN     BGR(0, 31, 0)
#define COLOR_VIOLET    BGR(31, 10, 31)
#define COLOR_BLACK     0

int
main () {
	hword *fb = (hword*)VRAM;
	Shape c[2];
	Shape b[20];
	Shape r;
	int i, key;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	delay_init();

	for (i = 0; i < 2; i++) {
		new_Circle(&c[i]);
		c[i].color = COLOR_WHITE;
		c[i].as.circle.set( &c[i], 70 + 20 * i, 70, 8 );
		c[i].v.set_v(&(c[i].v), 5, 5);
		c[i].v.set_a(&(c[i].v), 1, 1);
	}

	for (i = 0; i < 10; i++) {
		new_Box(&b[i]);
		b[i].color = COLOR_RED;
		b[i].as.box.set(&b[i], (20 + i * 20), 10, 10, 10);
		b[i].v.set_v(&(b[i].v), 0, 0);
		b[i].v.set_a(&(b[i].v), 0, 0);
		b[i].v.reflectable = 0;
		b[i].breakable = 1;
	}

	for (i = 10; i < 20; i++) {
		new_Box(&b[i]);
		b[i].color = COLOR_RED;
		b[i].as.box.set(&b[i], (20 + (i - 10) * 20), 30, 10, 10);
		b[i].v.set_v(&(b[i].v), 0, 0);
		b[i].v.set_a(&(b[i].v), 0, 0);
		b[i].v.reflectable = 0;
		b[i].breakable = 1;
	}

	new_Box(&r);
	r.color = COLOR_WHITE;
	r.as.box.set(&r, 120, 150, 50, 5);
	r.v.set_v(&(r.v), 0, 0);
	r.v.set_a(&(r.v), 0, 0);
	r.v.reflectable = 0;

	chain_shapes(23, &r, &c[0], &c[1], &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7], &b[8], &b[9], &b[10], &b[11], &b[12], &b[13], &b[14], &b[15], &b[16], &b[17], &b[18], &b[19] );
	
	wait_until_vblank();
	r.draw_all(&r);
	wait_while_vblank();
	
	while (1) {
		key = gba_register(KEY_STATUS);

		if (! (key & KEY_DOWN)) {
			r.v.set_a(&(r.v), r.v.ax, r.v.ax + 2);
		}
		else if (! (key & KEY_UP)) {
			r.v.set_a(&(r.v), r.v.ax, r.v.ax - 2);
		}
		else {
			r.v.set_a(&(r.v), r.v.ax, 0);
			r.v.set_v(&(r.v), r.v.dx, 0);
		}

		if (! (key & KEY_LEFT)) {
			r.v.set_a(&(r.v), r.v.ax - 2, r.v.ay);
		}
		else if (! (key & KEY_RIGHT)) {
			r.v.set_a(&(r.v), r.v.ax + 2, r.v.ay);
		}
		else {
			r.v.set_a(&(r.v), 0, r.v.ay);
			r.v.set_v(&(r.v), 0, r.v.dy);
		}

		r.run(&r);

		wait_until_vblank();
		r.redraw_all(&r);
		wait_while_vblank();

		delay(500);
	}
}

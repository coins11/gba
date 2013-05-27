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
#define COLOR_BLACK     0

int
main () {
	hword *fb = (hword*)VRAM;
	Shape b;
	Shape c[4];
	int i, key;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	delay_init();

	new_Circle(&c[0]);
	c[0].color = COLOR_WHITE;
	c[0].as.circle.set( &(c[0]), 50, 50, 8 );
	c[0].v.set_v(&(c[0].v), 5, 5);
	c[0].v.set_a(&(c[0].v), 0, 1);

	new_Circle(&c[1]);
	c[1].color = COLOR_RED;
	c[1].as.circle.set( &(c[1]), 120, 120, 8 );
	c[1].v.set_v(&(c[1].v), 1, 1);
	c[1].v.set_a(&(c[1].v), -1, -1);

	new_Circle(&c[2]);
	c[2].color = COLOR_BLUE;
	c[2].as.circle.set( &(c[2]), 80, 80, 10 );
	c[2].v.set_v(&(c[2].v), 3, 2);
	c[2].v.set_a(&(c[2].v), 0, -2);

	new_Circle(&c[3]);
	c[3].color = COLOR_GREEN;
	c[3].as.circle.set( &(c[3]), 150, 130, 12 );
	c[3].v.set_v(&(c[3].v), 9, 9);
	c[3].v.set_a(&(c[3].v), 0, 2);

	new_Box(&b);
	b.color = COLOR_WHITE;
	b.as.box.set(&b, 100, 100, 50, 5);
	b.v.set_v(&(b.v), 0, 0);
	b.v.set_a(&(b.v), 0, 0);
	b.v.reflectable = 0;

	chain_shapes(5, &c[0], &c[1], &c[2], &c[3], &b);
	
	wait_until_vblank();
	while (1) {
		for (i = 0; i < 4; i++) {
			c[i].draw(&c[i]);
		}

		b.draw(&b);
		wait_while_vblank();
		
		key = gba_register(KEY_STATUS);

		if (! (key & KEY_DOWN)) {
			b.v.set_a(&(b.v), b.v.ax, b.v.ax + 2);
		}
		else if (! (key & KEY_UP)) {
			b.v.set_a(&(b.v), b.v.ax, b.v.ax - 2);
		}
		else {
			b.v.set_a(&(b.v), b.v.ax, 0);
			b.v.set_v(&(b.v), b.v.dx, 0);
		}

		if (! (key & KEY_LEFT)) {
			b.v.set_a(&(b.v), b.v.ax - 2, b.v.ay);
		}
		else if (! (key & KEY_RIGHT)) {
			b.v.set_a(&(b.v), b.v.ax + 2, b.v.ay);
		}
		else {
			b.v.set_a(&(b.v), 0, b.v.ay);
			b.v.set_v(&(b.v), 0, b.v.dy);
		}

		c[0].run(&c[0]);

		delay(700);
	
		wait_until_vblank();

		for (i = 0; i < 4; i++) {
			c[i].erase(&c[i]);
		}

		b.erase(&b);
	}
}

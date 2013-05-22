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
	hword   *fb = (hword*)VRAM;
	Shape b;
	Shape c[4];
	int i, key;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0) = TMR_ENABLE + TMR_1024CLOCK;

	c[0].p.x = 50;
	c[0].p.y = 50;
	c[0].color = COLOR_WHITE;
	c[0].as.circle.r = 8;
	new_Circle(&c[0]);
	c[0].v.set_v(&(c[0].v), 5, 5);
	c[0].v.set_a(&(c[0].v), 0, 1);

	c[1].p.x = 120;
	c[1].p.y = 120;
	c[1].color = COLOR_RED;
	c[1].as.circle.r = 8;
	new_Circle(&c[1]);
	c[1].v.set_v(&(c[1].v), 1, 1);
	c[1].v.set_a(&(c[1].v), -1, -1);

	c[2].p.x = 80;
	c[2].p.y = 80;
	c[2].color = COLOR_BLUE;
	c[2].as.circle.r = 10;
	new_Circle(&c[2]);
	c[2].v.set_v(&(c[2].v), 3, 2);
	c[2].v.set_a(&(c[2].v), 0, -2);

	c[3].p.x = 150;
	c[3].p.y = 130;
	c[3].color = COLOR_GREEN;
	c[3].as.circle.r = 12;
	new_Circle(&c[3]);
	c[3].v.set_v(&(c[3].v), 9, 9);
	c[3].v.set_a(&(c[3].v), 0, 2);

	b.p.x = 100;
	b.p.y = 100;
	b.color = COLOR_WHITE;
	b.as.box.width  = 50;
	b.as.box.height = 5;
	new_Box(&b);
	b.v.set_v(&(b.v), 0, 0);
	b.v.set_a(&(b.v), 0, 0);

	c[0].next = &c[1];
	c[1].next = &c[2];
	c[2].next = &c[3];
	c[3].next = &b;
	b.next    = &c[0];
	
	while (1) {
		key = gba_register(KEY_STATUS);

		wait_until_vblank();

		for (i = 0; i < 4; i++) {
			c[i].draw(&c[i]);
		}

		b.draw(&b);
		wait_while_vblank();

		c[0].run(&c[0]);
		
		if (! (key & KEY_DOWN)) {
			b.move(&b, 0, 4);

			wait_while_vblank();
			b.erase(&b);
			wait_while_vblank();
		}

		if (! (key & KEY_UP)) {
			b.move(&b, 0, -4);

			wait_while_vblank();
			b.erase(&b);
		}

		if (! (key & KEY_LEFT)) {
			b.move(&b, -4, 0);

			wait_while_vblank();
			b.erase(&b);
			wait_while_vblank();
		}

		if (! (key & KEY_RIGHT)) {
			b.move(&b, 4, 0);

			wait_while_vblank();
			b.erase(&b);
			wait_while_vblank();
		}

		wait_until_vblank();

		for (i = 0; i < 4; i++) {
			c[i].erase(&c[i]);
		}

		wait_while_vblank();
	}
}

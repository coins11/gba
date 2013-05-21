#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/delay.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_RED       BGR(31, 31, 31)
#define COLOR_BLACK     0

const int __gba_multiboot;

int
main () {
	hword   *fb = (hword*)VRAM;
	Shape c, b;
	int key;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0) = TMR_ENABLE + TMR_1024CLOCK;

	c.p.x = 50;
	c.p.y = 50;
	c.color = COLOR_WHITE;
	c.as.circle.r = 8;
	new_Circle(&c);
	c.v.set_v(&(c.v), 5, 5);
	c.v.set_a(&(c.v), 0, 1);

	//c2.p.x = 80;
	//c2.p.y = 80;
	//c2.color = COLOR_RED;
	//c2.as.circle.r = 10;
	//new_Circle(&c2);
	//c2.v.set_v(&(c2.v), 3, 2);
	//c2.v.set_a(&(c2.v), 0, -2);

	b.p.x = 100;
	b.p.y = 100;
	b.color = COLOR_WHITE;
	b.as.box.width  = 50;
	b.as.box.height = 5;
	new_Box(&b);
	b.v.set_v(&(b.v), 0, 0);
	b.v.set_a(&(b.v), 0, 0);

	//b2.p.x = 0;
	//b2.p.y = 0;
	//b2.color = COLOR_RED;
	//b2.as.box.width  = 10;
	//b2.as.box.height = 10;
	//new_Box(&b2);
	//b2.v.set_v(&(b2.v), 10, 10);
	//b2.v.set_a(&(b2.v), 0, 0);

	//c.next = &b;
	//b.next = &c2;
	//c2.next = &c;
	
	c.next = &b;
	b.next = &c;


	//c.next = &b;
	//b.next = &c2;
	//c2.next = &b2;
	//b2.next = &c;

	while (1) {
		key = gba_register(KEY_STATUS);

		wait_until_vblank();
		c.draw(&c);
		b.draw(&b);
		//c2.draw(&c2);
		wait_while_vblank();

		c.run(&c);
		
		if (! (key & KEY_DOWN)) {
			b.move(&b, 0, 4);
			b.erase(&b);
		}

		if (! (key & KEY_UP)) {
			b.move(&b, 0, -4);
			b.erase(&b);
		}

		if (! (key & KEY_LEFT)) {
			b.move(&b, -4, 0);
			b.erase(&b);
		}

		if (! (key & KEY_RIGHT)) {
			b.move(&b, 4, 0);
			b.erase(&b);
		}

		wait_until_vblank();
		c.erase(&c);
		//c2.erase(&c2);
		wait_while_vblank();
	}
}

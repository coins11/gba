#include "lib/gba.h"
#include "lib/point.h"
#include "lib/screen.h"
#include "lib/velocity.h"
#include "lib/shape.h"
#include "lib/box.h"
#include "lib/circle.h"
#include "lib/delay.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_BLACK     0

int
main () {
	hword   *fb = (hword*)VRAM;
	Shape c, c2, b, b2;
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

	c2.p.x = 80;
	c2.p.y = 80;
	c2.color = COLOR_RED;
	c2.as.circle.r = 10;
	new_Circle(&c2);
	c2.v.set_v(&(c2.v), 5, 5);
	c2.v.set_a(&(c2.v), 1, 2);

	b.p.x = 60;
	b.p.y = 100;
	b.color = COLOR_WHITE;
	b.as.box.width  = 10;
	b.as.box.height = 10;
	new_Box(&b);
	b.v.set_v(&(b.v), 0, 0);
	b.v.set_a(&(b.v), 0, 1);

	b2.p.x = 0;
	b2.p.y = 0;
	b2.color = COLOR_RED;
	b2.as.box.width  = 10;
	b2.as.box.height = 10;
	new_Box(&b2);
	b2.v.set_v(&(b2.v), 10, 10);
	b2.v.set_a(&(b2.v), 0, 0);

	c.next = &b;
	b.next = &c;
	//b2.next = &c2;
	//c2.next = &c;
	
	while (1) {
		c.erase(&c);
		//c2.erase(&c2);
		b.erase(&b);
		//b2.erase(&b2);

		c.run(&c);

		c.draw(&c);
		//c2.draw(&c2);
		b.draw(&b);
		//b2.draw(&b2);
	}
}

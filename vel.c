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
	hword *fb = (hword *)VRAM;
	Circle c;
	Box b;

	gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0) = TMR_ENABLE + TMR_1024CLOCK;

	c.p.x = 50;
	c.p.y = 50;
	c.r   = 6;
	c.color = COLOR_WHITE;
	new_Circle(&c);

	c.v.set_v(&(c.v), 1, 1);

	b.p.x = 70;
	b.p.y = 70;
	b.width  = 10;
	b.height = 10;
	b.color = COLOR_WHITE;
	new_Box(&b);

	b.v.set_v((&b.v), -2, -1);

	while (1) {
		//c.run(&c);
		b.run(&b);

		b.draw(&b);
		//c.draw(&c);
		delay(600);
		//c.erase(&c);
		b.erase(&b);
	}
}

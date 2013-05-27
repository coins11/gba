#ifndef _DELAY_H_
#define _DELAY_H_

static inline void delay_init (void) {
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0)  = TMR_ENABLE + TMR_1024CLOCK;
}

extern inline void delay(hword);

extern inline void wait_until_vblank(void);
extern inline void wait_while_vblank(void);

#endif

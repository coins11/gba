#ifndef _DELAY_H_
#define _DELAY_H_

static inline void
delay_init (void) {
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0)  = TMR_ENABLE + TMR_1024CLOCK;
}

static inline void
delay(hword val)
{
	val += gba_register(TMR_COUNT0);
	while (val != gba_register(TMR_COUNT0));
}

static inline void 
wait_until_vblank(void) {
	while ((gba_register(LCD_STATUS) & 1) == 0);
}

static inline void 
wait_while_vblank(void) {
	while ((gba_register(LCD_STATUS) & 1));
}

#endif

#include "debug.h"

void vbaPrint(char *s)
{
    asm volatile("mov r0, %0;"
                 "swi 0xff0000;"
                 : // no ouput
                 : "r" (s)
                 : "r0");
}

void vbaIntPrint(int n)
{
	int i;
	char str[30];

	n = n & 0xFFFF;

	for (i = 0; i < 16; i++, n = n << 1) {
		if ( (n & 0x8000) >> 15 ) {
			str[i] = '1';
		} else {
			str[i] = '0';
		}
	}

	str[i]   = '\n';
	str[i+1] = '\0';

	vbaPrint(str);
}

#include "stdio.h"

struct hoge {
	int a[10];
	int b;
	int c;
};

void fuck (int *a) {
	printf("%d", *a);
}

int main () {
	struct hoge a;
	struct hoge *b;

	a.a[0] = 190;
	a.a[1] = 1;

	b = &a;

	fuck( &(b->a[0]) );

	return 1;
}

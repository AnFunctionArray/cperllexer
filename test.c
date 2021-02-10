int printf(const char*fmt, ...);

unsigned long a;

int b;

int c[2], d[4], f;



main() {
	b = 9UL;
	a = 9UL << 1 + 6;
	d[1] = c[0] + 1;
	f = 3;
	printf("hello world %d %p %p\n", f, d[1], &c + 1 );
	return 0;
}
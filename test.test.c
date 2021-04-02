
int printf (const char *fmt, ...);

int puts (const char *str);

int main() {
	int i, c;
	i = 6;
	c = 7;
	printf("%d %d %f\n", (c,i), (printf("hello\n"), i, c), (double)7.7);	
	return 0;
}
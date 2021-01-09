int printf(const char*fmt, ...);

main() {
	printf("hello world %d\n", 9UL << 1 + 6);
	return 0;
}

int printf (const char *fmt, ...);

int puts (const char *str);

int main() {
	int i;
	i = 0 + 1 || (puts("one\n"), puts(""))  && (puts("two\n")) || puts("");
	printf("%d\n", i);	
	return 0;
}

//i = if !(__hidden = 0 + 1) if __hidden = (puts("why\n"), puts()) if !(__hidden = puts("why\n")) __hidden = puts("");
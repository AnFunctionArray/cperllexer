int printf (const char *fmt, ...);

int b;

int c[2], d[4], f;

test (int a, int *p) { return printf ("%p %p %p\n", &*p, &p[0], *&*(p + 2)=7); }

main () {
    test(*c, c);
    return 0;
}
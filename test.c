int printf (const char *fmt, ...);

unsigned long a;

int b;

int c[2], d[4], f;

actualfun () {
    printf ("hello world %d %p %p\n", f, d[1], &c);
    return 0;
}

actualfun2 () { return 0; }

test (int *p) { return printf ("%p %p %p\n", &*p, &p[0], *&*(p + 2)=7); }

call (int pfun ()) { return (7 + pfun ()); }

main () {
    b = 9UL;
    a = 9UL << 1 + 6;
    f = 3;
    d[0] = d[1] = f + 5;
    int a, (*b[1])(int ());
	*b = call;
    a = 4;
    (&printf) ("hello world %d %p %p\n", (*b[0]) (actualfun), d, &d + 1);
    test (d);
    printf("%d %s\n", d[2], "string");
    return 0;
}
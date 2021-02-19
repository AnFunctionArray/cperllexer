int printf (const char *fmt, ...);

unsigned long a;

int b;

int c[2], d[4], f;

actualfun () {
    printf ("hello world %d %p %p\n", f, d[1], &c);
    return 0;
}

actualfun2 () { return 0; }

test (int a, int *p) { return printf ("%p %p %p %d\n", &*p, &p[0], *&*(p + 2)=7, a); }

call (int pfun ()) { return (7 + pfun ()); }

main () {
    b = 9UL;
    a = 9UL << 1 + 6;
    f = 3;
    d[0] = d[1] = f + 5;
    int a, (*b[1][2])(int ());
	**b = call;
    a = 4;
    (&printf) ("hello world %d %p %p\n", (*b[0]) (actualfun), d, &d + 1);
    test (9, d);
    printf("%d %d %c\n", d[2], sizeof b, **(&"test" + 1));
    return 0;
}
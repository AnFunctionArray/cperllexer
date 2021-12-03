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

notajokenow(struct notajoke{ int n; } n) {
    return printf("%d\n", n.n);
}

unsigned long long longlong;

int condtest() {
    int a, b, c;

    c = 7;
    b = 2;
    (a = 4 && 0) || b++ && c++;

    return printf("a %d, b %d c %d\n", a || b && 0, b, c);
}

main () {
    longlong = 18446744073709551615u;
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
    switch(a)
    {
        {
    default:
    case 4:;
        int a[2][2];

        a[1][1] = 5;

        printf("%p %p %d\n", a, *(a + 1) + 1, *(*(a + 1) + 1) + 2);

        struct test {
            int a, b;
            struct test* ptest[2];
        } test, test2[2];

        test.a = 4;
        test.b = 7;
        *test.ptest = &test;
        printf("%d %d\n", test.a, test.b);

        if (test.b != 7)
            printf("match\n");
        else if (test.b == 6) printf("no match\n");
        else printf("default\n");

        printf("end if\n");

        struct notajoke tmp;

        tmp.n = (*test.ptest)->a;

        long long last;

        last = 4294967295u + 1ll;

        double mid;

        mid = 3.14;

        int arr[2], *parr;

        int i;

        i = 0;

        typedef int testint;

        do {
            printf("%d\n", i++);
            if (i < 4) continue;
            printf("lower\n");
        }
        while (i < 8);

        testint obj;

        obj = 9;

        printf("obj - %d\n", obj);

        if(&i == 1) switch(i)
    case 1:
        break;

        parr = arr;

        arr[0] = arr[1] = 69;

        *(double*)&arr[1] = 77.7;

        printf("%u %d %d %f %llu %lld %d %d\n", 2u - 10, test.b, 1[*(int(*)[4])&test.a], mid--, 5UL - 2ULL, last++, *parr, *++parr);

        printf("%p %p %f\n", arr + 0, arr + 1, 1[*(double(((*)[2])))&arr]);

        printf("%f %lld\n", ++mid, last);

        notajokenow(tmp);
        }
    }
    return condtest();
}
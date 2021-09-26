typedef int test;

test const test, *test3, test4[2];

extern int printf(const char *, ...);

main() {
    return printf("%f\n", (double)9);
}
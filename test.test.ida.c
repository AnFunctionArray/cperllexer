
    extern int printf(const char*,...);
    typedef int a;
    int f(a b) {
        return printf("test\n"); 
    }
    int main() {
        a a;

        a = 'C';

        typedef int f;

        (f)(a);

        return printf("%c f\n", (a)a);
    }
typedef unsigned int uintptr_t;

uintptr_t printf(const char *pformat, ...);

uintptr_t f() {
        return 9;
}

uintptr_t main() {
        return printf("%d\n", f());
}
call (int pfun ()) { return (7 + pfun ()); }

main() {
        int a, (*b[1][2])(int ());
	**b = call;
}
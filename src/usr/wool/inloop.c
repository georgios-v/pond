int inloop(int n);

int inloop(int n) {
	int i, s = 0;

	for (i = 0; i < n; i++) {
		s += i;
	}

	return s;
}

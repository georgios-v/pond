
#include <stdio.h>
#include <stdlib.h>
#include <runtime0.h>

int skew(APP_ARGS);
extern int inloop(int);

TASK_PROTO_3(int, skew_tree, int, d, int, s, int, n);

TASK_3(int, skew_tree, int, d, int, s, int, n) {
	if (d > 0) {
		int r = 1, l = 1, a, b;
		if (s < 0) {
			r = -s;
		} else {
			l = s;
		}
		SPAWN(skew_tree, d - r, s, n);
		a = CALL(skew_tree, d - l, s, n);
		b = SYNC(skew_tree);
		return a + b;
	} else {
		inloop(n);
		return 1;
	}
}

int skew(APP_ARGS) {
	int i, d, n, m, s, r = 0;

	if (argc < 5) {
		fprintf(stderr, "Usage: stress <grain> <depth> <skew> <reps>\n");
		return 1;
	}

	n = atoi(argv[1]);
	d = atoi(argv[2]);
	s = atoi(argv[3]);
	m = atoi(argv[4]);

	for (i = 0; i < m; i++) {
		r = ROOT_CALL(skew_tree, d, s, n);
	}
	print("DONE, %d leaves per rep\n", r);

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <runtime0.h>

int fib(APP_ARGS);

TASK_PROTO_1(int, wool_pfib, int, n);

TASK_1(int, wool_pfib, int, n) {
	if (n < 2) {
		return n;
	} else {
		int m, k;
		SPAWN(wool_pfib, n - 1);
		k = CALL(wool_pfib, n - 2);
		m = SYNC(wool_pfib);
		return m + k;
	}
}

//TASK_2(int, fib, int, argc, char **, argv) {
int fib(APP_ARGS) {
	int n, m;

	if (argc < 2) {
		error("Usage: fib <woolopt>... <arg>\n");
		exit(2);
	}
	n = atoi(argv[ 1 ]);
	m = ROOT_CALL(wool_pfib, n);
	print("\n\n RESULT %d\n\n", m);
	return 0;
}


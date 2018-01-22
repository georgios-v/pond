#include "runtime0.h"
#include <stdio.h>
#include <stdlib.h>

int stress(APP_ARGS);
int inloop(int);

VOID_TASK_PROTO_2(tree, int, d, int, n);

VOID_TASK_2(tree, int, d, int, n) {
	if (d > 0) {
		SPAWN(tree, d - 1, n);
		CALL(tree, d - 1, n);
		SYNC(tree);
	} else {
		inloop(n);
	}
}

int stress(APP_ARGS) {
	//TASK_2( int, main, int, argc, char **, argv ) {
	int i, d, n, m;

	if (argc < 4) {
		fprintf(stderr, "Usage: stress [<wool opts>] <grain> <depth> <reps>\n");
		return 1;
	}

	n = atoi(argv[1]);
	d = atoi(argv[2]);
	m = atoi(argv[3]);

	for (i = 0; i < m; i++) {
		ROOT_CALL(tree, d, n);
	}
	print("DONE\n");

	return 0;
}

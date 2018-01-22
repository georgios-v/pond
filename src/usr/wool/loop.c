#include "runtime0.h"
#include <stdlib.h>
#include <stdio.h>

int loop(APP_ARGS);
extern int inloop(int);
LOOP_BODY_PROTO_1(work, 100, int, i, int, n);

LOOP_BODY_1(work, 100, int, i, int, n) {
	inloop(n);
}

int loop(APP_ARGS) {
	//TASK_2( int, main, int, argc, char **, argv ) {
	int grainsize = atoi(argv[1]);
	int p_iters = atoi(argv[2]);
	int s_iters = atoi(argv[3]);
	int i;
	
	for (i = 0; i < s_iters; i++) {
		ROOT_FOR(work, 0, p_iters, grainsize);
	}

	print("%d %d %d\n", grainsize, p_iters, s_iters);
	
	return 0;
}


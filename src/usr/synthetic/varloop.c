/*
 * File:   varloop.c
 * Author: Georgios Varisteas
 *
 * Created on October 5, 2012, 1:32 PM
 */
#include "runtime0.h"
#include <stdlib.h>
#include <stdio.h>

#define VARLOOP_FIXED_BASE 100

TASK_PROTO_1(int, wool_varloop_pfib, int, n);
TASK_PROTO_1(int, wool_varloop_sfib, int, n);
LOOP_BODY_PROTO_0( wool_varloop_work, 100, int, i);

int varloop(APP_ARGS);

TASK_1(int, wool_varloop_sfib, int, n) {
	verbose(" ** Executing sfib %d ** \n", n);
	if (n < 2) {
		return n;
	} else {
		int m, k;
		m = CALL(wool_varloop_sfib, (n - 1));
		k = CALL(wool_varloop_sfib, (n - 2));
		return (m + k);
	}
	return 0;
}

TASK_1(int, wool_varloop_pfib, int, n) {
	verbose(" ** Executing pfib %d ** \n", n);
	if (n < 2) {
		return n;
	} else {
		int m, k;
		SPAWN(wool_varloop_pfib, n - 1);
		k = CALL(wool_varloop_pfib, n - 2);
		m = SYNC(wool_varloop_pfib);
		return m + k;
	}
}
LOOP_BODY_0( wool_varloop_work, 100, int, i) {
	if(i % 2 == 0) {
		verbose(" *** ser %d\n", i);
		CALL(wool_varloop_sfib, (2*i));
	} else {
		verbose(" *** par %d\n", i);
		SPAWN(wool_varloop_pfib, i);
		SYNC(wool_varloop_pfib);
	}
	return;
}

int varloop(APP_ARGS) {
	if (argc < 2) {
		print("Usage: varloop <fibonacci>\n");
		return 1;
	}
	int n = atol(argv[1]);
	
	ROOT_FOR(wool_varloop_work, 0, n);
	
	print("\n\nVARLOOP END\n\n");

	return 0;
}


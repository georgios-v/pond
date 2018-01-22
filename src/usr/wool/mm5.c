#include "runtime0.h"
#include <stdio.h>
#include <stdlib.h>

// Blocks for cache
// More parallelism
// Repeats <n> times

LOOP_BODY_PROTO_6(mmj5, LARGE_BODY, int, jj, int, i, int, rows, int, bs, double*, a, double*, b, double*, c);
LOOP_BODY_PROTO_5(mm5, LARGE_BODY, int, ii, int, rows, int, bs, double*, a, double*, b, double*, c) ;

void mul_block(double* a, double* b, double* c, int rows, int bs, int ii, int jj, int kk) {
	int ilim = ii + bs <= rows ? ii + bs : rows;
	int i;
	//printf( "ii: %d jj %d kk: %d\n", ii, jj, kk );
	for (i = ii; i < ilim; i++) {
		int jlim = jj + bs <= rows ? jj + bs : rows;
		int j;
		for (j = jj; j < jlim; j++) {
			int klim = kk + bs <= rows ? kk + bs : rows;
			double sum = kk == 0 ? 0.0 : c[i * rows + j];
			int k;
			for (k = kk; k < klim; k++) {
				sum += a[i * rows + k] * b[k * rows + j];
			}
			c[i * rows + j] = sum;
		}
	}
}

LOOP_BODY_6(mmj5, LARGE_BODY, int, jj, int, i, int, rows, int, bs, double*, a, double*, b, double*, c) {
	int j = jj*bs, k;

	for (k = 0; k < rows; k += bs) {
		mul_block(a, b, c, rows, bs, i, j, k);
	}
}

LOOP_BODY_5(mm5, LARGE_BODY, int, ii, int, rows, int, bs, double*, a, double*, b, double*, c) {
	int i = ii*bs;

	FOR(mmj5, 0, (rows + bs - 1) / bs, i, rows, bs, a, b, c);
}

int mm5(APP_ARGS) {
	int i, j, ok;
	double *a, *b, *c;
	int rows, bs, reps;

	/* Decode arguments */

	if (argc < 4) {
		fprintf(stderr, "Usage: %s [wool options] <matrix rows> <blocksize> <repetitions>\n", argv[0]);
		exit(1);
	}
	rows = atoi(argv[1]);
	bs = atoi(argv[2]);
	reps = atoi(argv[3]);

#if 0
	printf("%d rows, blocksize %d\n", rows, bs);
	printf("mmj task size = %u, generic task size = %u\n",
			sizeof ( TD_TREE_mmj), sizeof ( Task));
#endif

	/* Allocate and initialize matrices */

	a = (double *) malloc(rows * rows * sizeof (double));
	b = (double *) malloc(rows * rows * sizeof (double));
	c = (double *) malloc(rows * rows * sizeof (double));

	for (i = 0; i < rows; i++) {
		for (j = 0; j < rows; j++) {
			a[i * rows + j] = 0.0;
			b[i * rows + j] = 0.0;
		}
		a[i * rows + i] = 1.0;
		b[i * rows + i] = 1.0;
	}


	/* Multiply matrices */

	for (i = 0; i < reps; i++) {
		ROOT_FOR(mm5, 0, (rows + bs - 1) / bs, rows, bs, a, b, c);
	}

	/* Check result */

	ok = 1;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < rows; j++) {
			if (i != j && c[i * rows + j] != 0.0) {
				ok = 0;
			}
			if (i == j && c[i * rows + j] != 1.0) {
				ok = 0;
			}
		}
	}

	print("Ok: %d\n", ok);
	return 0;
}

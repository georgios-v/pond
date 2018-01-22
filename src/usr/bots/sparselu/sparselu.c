/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../common/bots.h"
#include "sparselu.h"

VOID_TASK_PROTO_2(bdiv, float*, diag, float*, row);
VOID_TASK_PROTO_2(fwd, float*, diag, float*, col);
VOID_TASK_PROTO_4(cilk_task, int, ii, int, jj, int, kk, float**, BENCH);
VOID_TASK_PROTO_1(sparselu_par_call_t, float**, BENCH);
#if defined(FOR_VERSION)
LOOP_BODY_PROTO_2(iteration_fwd, 10000, int, jj, int, kk, float**, BENCH);
LOOP_BODY_PROTO_2(iteration_bdiv, 10000, int, jj, int, kk, float**, BENCH);
LOOP_BODY_PROTO_2(iteration_last, 10000, int, ii, int, kk, float**, BENCH);
#endif
/***********************************************************************
 * checkmat:
 **********************************************************************/
int checkmat(float *M, float *N) {
	int i, j;
	float r_err;

	for (i = 0; i < bots_arg_size_1; i++) {
		for (j = 0; j < bots_arg_size_1; j++) {
			r_err = M[i * bots_arg_size_1 + j] - N[i * bots_arg_size_1 + j];
			if (r_err < 0.0) r_err = -r_err;
			r_err = r_err / M[i * bots_arg_size_1 + j];
			if (r_err > EPSILON) {
				bots_message("Checking failure: A[%d][%d]=%f  B[%d][%d]=%f; Relative Error=%f\n",
						i, j, M[i * bots_arg_size_1 + j], i, j, N[i * bots_arg_size_1 + j], r_err);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/***********************************************************************
 * genmat:
 **********************************************************************/
void genmat(float *M[]) {
	int null_entry, init_val, i, j, ii, jj;
	float *p;

	init_val = 1325;

	/* generating the structure */
	for (ii = 0; ii < bots_arg_size; ii++) {
		for (jj = 0; jj < bots_arg_size; jj++) {
			/* computing null entries */
			null_entry = FALSE;
			if ((ii < jj) && (ii % 3 != 0)) null_entry = TRUE;
			if ((ii > jj) && (jj % 3 != 0)) null_entry = TRUE;
			if (ii % 2 == 1) null_entry = TRUE;
			if (jj % 2 == 1) null_entry = TRUE;
			if (ii == jj) null_entry = FALSE;
			if (ii == jj - 1) null_entry = FALSE;
			if (ii - 1 == jj) null_entry = FALSE;
			/* allocating matrix */
			if (null_entry == FALSE) {
				M[ii * bots_arg_size + jj] = (float *) malloc(bots_arg_size_1 * bots_arg_size_1 * sizeof (float));
				if ((M[ii * bots_arg_size + jj] == NULL)) {
					bots_message("Error: Out of memory\n");
					exit(101);
				}
				/* initializing matrix */
				p = M[ii * bots_arg_size + jj];
				for (i = 0; i < bots_arg_size_1; i++) {
					for (j = 0; j < bots_arg_size_1; j++) {
						init_val = (3125 * init_val) % 65536;
						(*p) = (float) ((init_val - 32768.0) / 16384.0);
						p++;
					}
				}
			} else {
				M[ii * bots_arg_size + jj] = NULL;
			}
		}
	}
}

/***********************************************************************
 * print_structure:
 **********************************************************************/
void print_structure(char *name, float *M[]) {
	int ii, jj;
	bots_message("Structure for matrix %s @ 0x%p\n", name, M);
	for (ii = 0; ii < bots_arg_size; ii++) {
		for (jj = 0; jj < bots_arg_size; jj++) {
			if (M[ii * bots_arg_size + jj] != NULL) {
				bots_message("x");
			} else bots_message(" ");
		}
		bots_message("\n");
	}
	bots_message("\n");
}

/***********************************************************************
 * allocate_clean_block:
 **********************************************************************/
float * allocate_clean_block(void) {
	int i, j;
	float *p, *q;

	p = (float *) malloc(bots_arg_size_1 * bots_arg_size_1 * sizeof (float));
	q = p;
	if (p != NULL) {
		for (i = 0; i < bots_arg_size_1; i++)
			for (j = 0; j < bots_arg_size_1; j++) {
				(*p) = 0.0;
				p++;
			}

	} else {
		bots_message("Error: Out of memory\n");
		exit(101);
	}
	return (q);
}

/***********************************************************************
 * lu0:
 **********************************************************************/
void lu0(float *diag) {
	int i, j, k;

	for (k = 0; k < bots_arg_size_1; k++)
		for (i = k + 1; i < bots_arg_size_1; i++) {
			diag[i * bots_arg_size_1 + k] = diag[i * bots_arg_size_1 + k] / diag[k * bots_arg_size_1 + k];
			for (j = k + 1; j < bots_arg_size_1; j++)
				diag[i * bots_arg_size_1 + j] = diag[i * bots_arg_size_1 + j] - diag[i * bots_arg_size_1 + k] * diag[k * bots_arg_size_1 + j];
		}
}

/***********************************************************************
 * bdiv:
 **********************************************************************/
//void bdiv(float *diag, float *row)

VOID_TASK_2(bdiv, float*, diag, float*, row) {
	int i, j, k;
	for (i = 0; i < bots_arg_size_1; i++)
		for (k = 0; k < bots_arg_size_1; k++) {
			row[i * bots_arg_size_1 + k] = row[i * bots_arg_size_1 + k] / diag[k * bots_arg_size_1 + k];
			for (j = k + 1; j < bots_arg_size_1; j++)
				row[i * bots_arg_size_1 + j] = row[i * bots_arg_size_1 + j] - row[i * bots_arg_size_1 + k] * diag[k * bots_arg_size_1 + j];
		}
}

void bdiv(float *diag, float *row) {
	int i, j, k;
	for (i = 0; i < bots_arg_size_1; i++)
		for (k = 0; k < bots_arg_size_1; k++) {
			row[i * bots_arg_size_1 + k] = row[i * bots_arg_size_1 + k] / diag[k * bots_arg_size_1 + k];
			for (j = k + 1; j < bots_arg_size_1; j++)
				row[i * bots_arg_size_1 + j] = row[i * bots_arg_size_1 + j] - row[i * bots_arg_size_1 + k] * diag[k * bots_arg_size_1 + j];
		}
}

/***********************************************************************
 * bmod:
 **********************************************************************/
void bmod(float *row, float *col, float *inner) {
	int i, j, k;
	for (i = 0; i < bots_arg_size_1; i++)
		for (j = 0; j < bots_arg_size_1; j++)
			for (k = 0; k < bots_arg_size_1; k++)
				inner[i * bots_arg_size_1 + j] = inner[i * bots_arg_size_1 + j] - row[i * bots_arg_size_1 + k] * col[k * bots_arg_size_1 + j];
}
/***********************************************************************
 * fwd:
 **********************************************************************/
//void fwd(float *diag, float *col)

VOID_TASK_2(fwd, float*, diag, float*, col) {
	int i, j, k;
	for (j = 0; j < bots_arg_size_1; j++)
		for (k = 0; k < bots_arg_size_1; k++)
			for (i = k + 1; i < bots_arg_size_1; i++)
				col[i * bots_arg_size_1 + j] = col[i * bots_arg_size_1 + j] - diag[i * bots_arg_size_1 + k] * col[k * bots_arg_size_1 + j];
}

void fwd(float *diag, float *col) {
	int i, j, k;
	for (j = 0; j < bots_arg_size_1; j++)
		for (k = 0; k < bots_arg_size_1; k++)
			for (i = k + 1; i < bots_arg_size_1; i++)
				col[i * bots_arg_size_1 + j] = col[i * bots_arg_size_1 + j] - diag[i * bots_arg_size_1 + k] * col[k * bots_arg_size_1 + j];
}

void sparselu_init(float ***pBENCH, char *pass) {
	*pBENCH = (float **) malloc(bots_arg_size * bots_arg_size * sizeof (float *));
	genmat(*pBENCH);
	print_structure(pass, *pBENCH);
}

//void cilk_task(int ii,int jj, int kk, float **BENCH)

VOID_TASK_4(cilk_task, int, ii, int, jj, int, kk, float**, BENCH) {
	if (BENCH[ii * bots_arg_size + jj] == NULL) BENCH[ii * bots_arg_size + jj] = allocate_clean_block();
	bmod(BENCH[ii * bots_arg_size + kk], BENCH[kk * bots_arg_size + jj], BENCH[ii * bots_arg_size + jj]);
}

#if defined(FOR_VERSION)
LOOP_BODY_2(iteration_fwd, 10000, int, jj, int, kk, float**, BENCH) {
	if (BENCH[kk * bots_arg_size + jj] != NULL) CALL(fwd, BENCH[kk * bots_arg_size + kk], BENCH[kk * bots_arg_size + jj]);
	if (BENCH[jj * bots_arg_size + kk] != NULL) CALL(bdiv, BENCH[kk * bots_arg_size + kk], BENCH[jj * bots_arg_size + kk]);
}

LOOP_BODY_2(iteration_bdiv, 10000, int, jj, int, kk, float**, BENCH) {
	if (BENCH[jj * bots_arg_size + kk] != NULL) CALL(bdiv, BENCH[kk * bots_arg_size + kk], BENCH[jj * bots_arg_size + kk]);
}

LOOP_BODY_2(iteration_last, 10000, int, ii, int, kk, float**, BENCH) {
	int jj;
	if (BENCH[ii * bots_arg_size + kk] != NULL)
		for (jj = kk + 1; jj < bots_arg_size; jj++)
			if (BENCH[kk * bots_arg_size + jj] != NULL)
				CALL(cilk_task, ii, jj, kk, BENCH);
}
#endif


//void sparselu_par_call_t(float **BENCH)

VOID_TASK_1(sparselu_par_call_t, float**, BENCH) {
	int ii, jj, kk;
	int fwds, bdivs, tsk, i;

	bots_message("Computing SparseLU Factorization (%dx%d matrix with %dx%d blocks) ",
			bots_arg_size, bots_arg_size, bots_arg_size_1, bots_arg_size_1);
	//#pragma omp parallel
	//#pragma omp single nowait
	//#pragma omp task untied

#if defined(FOR_VERSION)

	for (kk = 0; kk < bots_arg_size; kk++) {
		bdivs = 0;
		fwds = 0;
		tsk = 0;
		lu0(BENCH[kk * bots_arg_size + kk]);

		FOR(iteration_fwd, kk + 1, bots_arg_size, kk, BENCH);

		//      FOR(iteration_bdiv , kk+1 , bots_arg_size, kk , BENCH);

		//     for (ii=kk+1; ii<bots_arg_size; ii++)
		//    for (i =0;i<bdivs;i++) SYNC(bdiv);

		FOR(iteration_last, kk + 1, bots_arg_size, kk, BENCH);

		//      for (ii=kk+1; ii<bots_arg_size; ii++)


		//for (i =0;i<tsk;i++) SYNC(cilk_task);

	}

#else

	for (kk = 0; kk < bots_arg_size; kk++) {
		bdivs = 0;
		fwds = 0;
		tsk = 0;
		lu0(BENCH[kk * bots_arg_size + kk]);

		for (jj = kk + 1; jj < bots_arg_size; jj++)
			if (BENCH[kk * bots_arg_size + jj] != NULL) {
				SPAWN(fwd, BENCH[kk * bots_arg_size + kk], BENCH[kk * bots_arg_size + jj]);
				fwds++;
			}

		for (ii = kk + 1; ii < bots_arg_size; ii++)
			if (BENCH[ii * bots_arg_size + kk] != NULL) {
				SPAWN(bdiv, BENCH[kk * bots_arg_size + kk], BENCH[ii * bots_arg_size + kk]);
				bdivs++;
			}

		for (i = 0; i < bdivs; i++) SYNC(bdiv);
		for (i = 0; i < fwds; i++) SYNC(fwd);


		for (ii = kk + 1; ii < bots_arg_size; ii++)
			if (BENCH[ii * bots_arg_size + kk] != NULL)
				for (jj = kk + 1; jj < bots_arg_size; jj++)
					if (BENCH[kk * bots_arg_size + jj] != NULL) {
						SPAWN(cilk_task, ii, jj, kk, BENCH);
						tsk++;
					}


		for (i = 0; i < tsk; i++) SYNC(cilk_task);
	}

#endif

	bots_message(" completed!\n");
}

void sparselu_par_call(opt_worker_t self, float **BENCH) {
	ROOT_CALL(sparselu_par_call_t, BENCH);
}

void sparselu_seq_call(float **BENCH) {
	int ii, jj, kk;

	for (kk = 0; kk < bots_arg_size; kk++) {
		lu0(BENCH[kk * bots_arg_size + kk]);
		for (jj = kk + 1; jj < bots_arg_size; jj++)
			if (BENCH[kk * bots_arg_size + jj] != NULL) {
				fwd(BENCH[kk * bots_arg_size + kk], BENCH[kk * bots_arg_size + jj]);
			}
		for (ii = kk + 1; ii < bots_arg_size; ii++)
			if (BENCH[ii * bots_arg_size + kk] != NULL) {
				bdiv(BENCH[kk * bots_arg_size + kk], BENCH[ii * bots_arg_size + kk]);
			}
		for (ii = kk + 1; ii < bots_arg_size; ii++)
			if (BENCH[ii * bots_arg_size + kk] != NULL)
				for (jj = kk + 1; jj < bots_arg_size; jj++)
					if (BENCH[kk * bots_arg_size + jj] != NULL) {
						if (BENCH[ii * bots_arg_size + jj] == NULL) BENCH[ii * bots_arg_size + jj] = allocate_clean_block();
						bmod(BENCH[ii * bots_arg_size + kk], BENCH[kk * bots_arg_size + jj], BENCH[ii * bots_arg_size + jj]);
					}

	}
}

void sparselu_fini(float **BENCH, char *pass) {
	print_structure(pass, BENCH);
}

int sparselu_check(float **seq, float **BENCH) {
	int ii, jj, ok = 1;

	for (ii = 0; ((ii < bots_arg_size) && ok); ii++) {
		for (jj = 0; ((jj < bots_arg_size) && ok); jj++) {
			if ((seq[ii * bots_arg_size + jj] == NULL) && (BENCH[ii * bots_arg_size + jj] != NULL)) ok = FALSE;
			if ((seq[ii * bots_arg_size + jj] != NULL) && (BENCH[ii * bots_arg_size + jj] == NULL)) ok = FALSE;
			if ((seq[ii * bots_arg_size + jj] != NULL) && (BENCH[ii * bots_arg_size + jj] != NULL))
				ok = checkmat(seq[ii * bots_arg_size + jj], BENCH[ii * bots_arg_size + jj]);
		}
	}
	if (ok) return BOTS_RESULT_SUCCESSFUL;
	else return BOTS_RESULT_UNSUCCESSFUL;
}

int sparselu(APP_ARGS) {
	//Launch thread in the beginning of WOOL

	cycles_t bots_t_start;
	cycles_t bots_t_end;

//	print("\n %d -- %s %s \n", argc, argv[0], argv[1]);
	bots_arg_size = 50;
	bots_arg_size_1 = 100;
	bots_check_flag = BOTS_CONFIG;
	bots_sequential_flag = BOTS_SEQUENTIAL;

	float **seq, **BENCH;
	bots_set_info("SparseLU (Single version)", false);

	if (bots_sequential_flag || bots_check_flag) {
		bots_sequential_flag = 1;
		sparselu_init(&seq, "serial");

		bots_t_start = bots_usecs();
		sparselu_seq_call(seq);
		bots_t_end = bots_usecs();
		bots_time_sequential = bots_t_end - bots_t_start;

		sparselu_fini(seq, "serial");
	}

	sparselu_init(&BENCH, "benchmark");

	bots_t_start = bots_usecs();
	sparselu_par_call(self, BENCH);
	bots_t_end = bots_usecs();
	bots_time_program = bots_t_end - bots_t_start;

	sparselu_fini(BENCH, "benchmark");

	if (bots_check_flag) {
		bots_result = sparselu_check(seq, BENCH);
	}

	free(seq);
	free(BENCH);

	bots_print_results();

	//Join the threads together
	return (0);
}


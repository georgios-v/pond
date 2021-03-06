#ifndef SPARSELU_H
#define SPARSELU_H

#define EPSILON 1.0E-6

int checkmat (float *M, float *N);
void genmat (float *M[]);
void print_structure(char *name, float *M[]);
float * allocate_clean_block(void);
void lu0(float *diag);
void bdiv(float *diag, float *row);
void bmod(float *row, float *col, float *inner);
void fwd(float *diag, float *col);

void sparselu_init(float ***pM, char *pass);
void sparselu_fini(float **M, char *pass);
void sparselu_seq_call(float **seq);
void sparselu_par_call(opt_worker_t self, float **BENCH);
int sparselu_check(float **seq, float **BENCH);
int sparselu(APP_ARGS);

#endif

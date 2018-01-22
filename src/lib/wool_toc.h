/* 
 * File:   toc.h
 * Author: sparc
 *
 * Created on January 28, 2013, 7:10 PM
 */

#ifndef WOOL_TOC_H
#define	WOOL_TOC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "pond_types.h"

int varloop(APP_ARGS);

int fib(APP_ARGS);
int loop(APP_ARGS);
int mm5(APP_ARGS);
int mm7(APP_ARGS);
int stress(APP_ARGS);
int skew(APP_ARGS);

int fft(APP_ARGS);
int bfib(APP_ARGS);
int floorplan(APP_ARGS);
int nqueens(APP_ARGS);
int sort(APP_ARGS);
int sparselu(APP_ARGS);
int strassen(APP_ARGS);
int uts(APP_ARGS);

int matmul(APP_ARGS);
int lu(APP_ARGS);
int knapsack(APP_ARGS);


#ifdef	__cplusplus
}
#endif

#endif	/* WOOL_TOC_H */


/*
 * File:   toc.h
 * Author: Georgios Varisteas
 *
 * Created on May 30, 2012, 1:28 PM
 */

#ifndef TOC_H
#define	TOC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <pond_types.h>

#include "wool_toc.h"

#define DEFAULT_RT_SET "padvw"

	extern struct __runtime_vtbl vtbl_wool0_asteal;
	extern struct __runtime_vtbl vtbl_wool0_dsteal;
	extern struct __runtime_vtbl vtbl_wool0_palirria;;
	extern struct __runtime_vtbl vtbl_wool0_dvs;
	extern struct __runtime_vtbl vtbl_wool0;

	struct __pond_rt rts[] = {
		{"ASTEAL", &vtbl_wool0_asteal, 'a'},
		{"ASTEAL", &vtbl_wool0_dsteal, 'd'},
		{"PALIRRIA", &vtbl_wool0_palirria, 'p'},
		{"ASTEAL", &vtbl_wool0_dsteal, 'v'},
		{"WOOL", &vtbl_wool0, 'w'},
	};

	struct __pond_app apps[] = {
		{"varloop", varloop, DEFAULT_RT_SET, "Synthetic Loop"}, // varloop <fibonacci> : varloop 40
		{"fib", fib, DEFAULT_RT_SET, "WOOL Fibonacci"}, // fib <number> : fib 40
		{"loop", loop, DEFAULT_RT_SET, "WOOL Loop"}, // loop <grainsize> <p_iters> <s_iters> : loop 1000 40 10
		{"stress", stress, DEFAULT_RT_SET, "WOOL Stress"}, // stress <grain> <depth> <reps> : stress 1000 5 15
		{"mm5", mm5, DEFAULT_RT_SET, "WOOL mm5"}, // mm5 <matrix rows> <blocksize> <repetitions> : mm7 1000 64 5
		{"mm7", mm7, DEFAULT_RT_SET, "WOOL mm7"}, // mm7 <matrix rows> <repetitions> : mm7 200 3
		{"skew", skew, DEFAULT_RT_SET, "WOOL Skew"}, // skew 20000 20 5 50
		{"fft", fft, DEFAULT_RT_SET, "BOTS FFT"}, // fft
		{"bfib", bfib, DEFAULT_RT_SET, "BOTS Fib"},
		{"floorplan", floorplan, DEFAULT_RT_SET, "BOTS Floorplan"},
		{"nqueens", nqueens, DEFAULT_RT_SET, "BOTS nQueens"},
		{"sort", sort, DEFAULT_RT_SET, "BOTS sort"},
		{"sparselu", sparselu, DEFAULT_RT_SET, "BOTS sparselu"},
		{"strassen", strassen, DEFAULT_RT_SET, "BOTS strassen"},
		{"uts", uts, DEFAULT_RT_SET, "BOTS UTS"},
		{"knapsack", knapsack, DEFAULT_RT_SET, "Cilk knapsack"},
		{"lu", lu, DEFAULT_RT_SET, "Cilk lu"},
		{"matmul", matmul, DEFAULT_RT_SET, "Cilk matmul"},
	};


#ifdef	__cplusplus
}
#endif

#endif	/* TOC_H */


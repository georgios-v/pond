/* 
 * File:   bench.h
 * Author: georgios
 *
 * Created on January 21, 2013, 2:41 PM
 */

#ifndef POND_BENCH_H
#define	POND_BENCH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#ifdef BARRELFISH
#include <bench/bench.h>
#include <bench/bench_arch.h>
#else
#include <pond_types.h>
#define BENCH_IGNORE_WATERMARK 0XDEADBEEF
	void bench_init(void);
	cycles_t bench_tscoverhead(void);
	uint64_t bench_tsc_to_ms(cycles_t tsc);
	/** \brief This code reads the cycle counter */
	static inline uint64_t bench_tsc(void) {
		uint32_t eax = 0, edx = 0;
		__asm volatile ("rdtsc" : "=a" (eax), "=d" (edx));
		return ((uint64_t)edx << 32) | eax;
	}
#endif

	typedef struct __pond_worker_bench *worker_bench_t;

#define tsc_get (bench_tsc() - bench_tscoverhead())
#define tsc_diff(_tsc) (tsc_get - _tsc)

	/* add to the per-worker total execution time value */
	void stat_total_add(worker_bench_t bnc, cycles_t tsc);
	/* set the worker's system time start timestamp */
	void stat_system_start(worker_bench_t bnc, cycles_t tsc);
	/* set the worker's system time start timestamp */
	void stat_system_stop(worker_bench_t bnc, cycles_t tsc);

	/* print summary of timestamps */
	void stat_print_bench(char str[], coreid_t idx, worker_bench_t bnc, rt_bnc_func bnc_f, opt_worker_t w);

	/* allocate and initialize new bench structure */
	worker_bench_t stat_bench_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* POND_BENCH_H */

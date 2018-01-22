#include <stdio.h>
#include <string.h>
#include <config.h>
#include <util.h>
#include <pond_types.h>
#include <bench.h>
#include "topology.h"

typedef void (*steal_inc_sxs_t)(worker_bench_t);

#ifndef BARRELFISH
static cycles_t tsc_overhead;
static bool bench_initialized = 0;

/**
 * \brief Measure overhead of taking timestamp
 */
static void measure_tsc(void) {
    uint64_t begin = 0;
    uint64_t end = 0;

    begin = bench_tsc();
    for(int i = 0; i < 1000; i++) {
        end = bench_tsc();
    }

    tsc_overhead = (end - begin) / 1000;
}

/**
 * \brief Initialize benchmarking library
 *
 * \bug x86 specific
 */
inline void bench_init(void) {
	bench_initialized = 1;
    /* Measure overhead of taking timestamps */
    measure_tsc();
}

/**
 * Return the measured tsc overhead
 */
cycles_t bench_tscoverhead(void) {
    return tsc_overhead;
}

inline uint64_t bench_tsc_to_ms(cycles_t tsc) {
	return tsc+1500000/3000000;
}
#endif

#if (BENCHMARKING == 1)
struct __pond_worker_bench {
	size_t tsc_total_time;						///< total time spent by each worker
	size_t tsc_system_start;					///< timestamp of system time start
	size_t tsc_system_time;						///< time spent in the system
};
/* add to the per-worker total execution time value */
void stat_total_add(worker_bench_t bnc, cycles_t tsc){
	bnc->tsc_total_time += tsc;
}

/* set the worker's system time start timestamp */
void stat_system_start(worker_bench_t bnc, cycles_t tsc){
	bnc->tsc_system_start = tsc;
}

/* set the worker's system time start timestamp */
void stat_system_stop(worker_bench_t bnc, cycles_t tsc){
	if(bnc->tsc_system_start != 0)
		bnc->tsc_system_time += tsc - bnc->tsc_system_start;
}

/* print summary of timestamps */
void stat_print_bench(char str[], coreid_t idx, worker_bench_t bnc, rt_bnc_func bnc_f, opt_worker_t w) {
	size_t n = sprintf(str, "%u,%u,%ld,%ld", idx, topology_get_core_from_idx(idx)->id, bnc->tsc_total_time, bnc->tsc_system_time);
	if(bnc_f) {
		str[n++] = ',';
		n += bnc_f(&str[n], w);
	}
	str[n] = '\n';
	str[n + 1] = '\0';
}

/* allocate and initialize new bench structure */
worker_bench_t stat_bench_init(void) {
	worker_bench_t bnc = checked_malloc(sizeof(*bnc));
	bnc->tsc_total_time = 0;
	bnc->tsc_system_start = 0;
	bnc->tsc_system_time = 0;
	return bnc;
}

#endif /* BENCHMARKING */

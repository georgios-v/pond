/*
 * File:   bench.h
 * Author: Georgios Varisteas
 *
 * Created on May 16, 2014, 1:43 PM
 */

#ifndef WOOL0_BENCH_H
#define	WOOL0_BENCH_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <pond_types.h>

	typedef struct wool0_bench {
		cycles_t non_useful_time;
		cycles_t tasks;
		cycles_t steals_s;
		cycles_t steals_f;
	} *wool0_bench_t;

	size_t __wool0_get_bench_string (char *buffer, opt_worker_t w);

#ifdef	__cplusplus
}
#endif

#endif	/* WOOL0_BENCH_H */


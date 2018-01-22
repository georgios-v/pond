/*
 * File:   bench.c
 * Author: Georgios Varisteas
 *
 * Created on May 16, 2014, 1:42 PM
 */

#include <config.h>
#include "types.h"
#include "bench.h"

size_t __wool0_get_bench_string (char *buffer, opt_worker_t w) {
	wool0_bench_t bnc = (wool0_bench_t)((opt_worker_wool0_t)w)->bnc;
	if(buffer == NULL)
		return 0;
	buffer[0] = '\0';
	size_t n = sprintf(buffer, "%lu,%lu,%lu,%lu", bnc->non_useful_time, bnc->tasks, bnc->steals_s, bnc->steals_f);
	return n;
}

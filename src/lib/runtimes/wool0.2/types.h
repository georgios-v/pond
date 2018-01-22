/* 
 * File:   wool_types.h
 * Author: georgios
 *
 * Created on March 25, 2013, 8:03 PM
 */

#ifndef WOOL0_TYPES_H
#define	WOOL0_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef TYPES_INCLUDED
#define TYPES_INCLUDED__
#endif
#ifndef TYPES_INCLUDED__
	typedef struct opt_worker_wool0 Worker;
#else
#undef TYPES_INCLUDED__
#endif
#include <config.h>
#include <wool0.h>
#include "bench.h"

#define INIT_WORKER_DQ_SIZE 100000
#define INIT_N_STEALABLE 20



	typedef void (*__sync_func)(volatile Task *, balarm_t);

	typedef struct opt_worker_wool0 {
		coreid_t idx;
		// First cache line, public stuff seldom written by the owner
		Task *dq_base, // Always pointing the base of the dequeue
				*dq_bot; // The next task to steal
		wool_lock_t *dq_lock; // Mainly used for mutex among thieves, but also as backup for victim
		wool_lock_t the_lock; // dq_lock points here
		__sync_func sync;
		char pad1[ PAD(sizeof(coreid_t) + 3*P_SZ + sizeof (wool_lock_t) + sizeof(__sync_func), LINE_SIZE) ];

		// Second cache line, private stuff often written by the owner
		int dq_size;
		Task *dq_top;			// top gets updated in SPAWN and SYNC
#if (BENCHMARKING == 1)
		wool0_bench_t bnc;
#endif
		/* victim selection specific arguments */
		int i;
		coreid_t victim_idx;
#if (BENCHMARKING == 1)
		char pad2[ PAD(2*P_SZ + 2*I_SZ + sizeof(coreid_t), LINE_SIZE) ];
#else
		char pad2[ PAD(P_SZ + 2*I_SZ + sizeof(coreid_t), LINE_SIZE) ];
#endif
	} *opt_worker_wool0_t;

	typedef unsigned int opt_program_wool0_t;

#ifdef	__cplusplus
}
#endif

#endif	/* WOOL0_TYPES_H */


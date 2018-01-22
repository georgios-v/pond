/*
 * File:   types.h
 * Author: Georgios Varisteas
 *
 * Created on March 10, 2014, 1:17 PM
 */

#ifndef WOOL0_DVS_TYPES_H
#define	WOOL0_DVS_TYPES_H

#ifdef TYPES_INCLUDED
#define TYPES_INCLUDED__
#else
#define TYPES_INCLUDED
#endif
#ifndef TYPES_INCLUDED__
	typedef struct opt_worker_dvs Worker;
#else
#undef TYPES_INCLUDED__
#endif

#include <config.h>
#include "../types.h"
#include <wool0.h>

#ifdef	__cplusplus
extern "C" {
#endif

///< the maximum size of the VO set is always 4 times the dimensions + 4 (2 + self + terminator)
#define MAX_V_SZ 7

	typedef struct wool0_bench *wool0_dvs_bench_t;

	typedef struct opt_worker_dvs *opt_worker_dvs_t;
	struct opt_worker_dvs {
		coreid_t idx;
		// First cache line, public stuff seldom written by the owner
		Task *dq_base,			// Always pointing the base of the dequeue
				*dq_bot;		// The next task to steal
		wool_lock_t *dq_lock;	// Mainly used for mutex among thieves, but also as backup for victim
		wool_lock_t the_lock;	// dq_lock points here
		__sync_func sync;
		char pad1[ PAD(sizeof(coreid_t) + 3*P_SZ + sizeof (wool_lock_t) + sizeof(__sync_func), LINE_SIZE) ];

		// Second cache line, private stuff often written by the owner
		int dq_size;
		Task *dq_top;			// top gets updated in SPAWN and SYNC
#if (BENCHMARKING == 1)
		wool0_dvs_bench_t bnc;
#endif
		/* victim selection specific arguments */

		coreid_t Vid;			///< Victims list version identifier, incremented on every DMC, on change abort steal
		coreid_t V_sz;			///< Victims set current size
		opt_worker_dvs_t *V;	///< The victims list. Pre-allocated to MAX_VICTIMS (2m - 1), terminated by the ARRAY_TERMINATOR macro.

#if (BENCHMARKING == 1)
		char pad2[ PAD(3*P_SZ + I_SZ + 2*sizeof(coreid_t), LINE_SIZE) ];
#else
		char pad2[ PAD(2*P_SZ + I_SZ + 2*sizeof(coreid_t), LINE_SIZE) ];
#endif
	};

	typedef opt_program_wool0_t opt_program_wool0_dvs_t;

#ifdef	__cplusplus
}
#endif

#endif	/* WOOL0_DVS_TYPES_H */


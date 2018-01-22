/*
 * File:   wool.c
 * Author: Georgios Varisteas
 *
 * Created on January 18, 2013, 12:26 PM
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pond_types.h>
#include "types.h"
#include <wool0.h>
#include <bench.h>


/* Implements
   leapfrogging,
   private tasks (fixed depth),
   peek (check stealable before locking),
   trylock (steal from other worker if other thief busy),
   linear stealing (rather than always random)

   Does not implement
   Resizing or overflow checking of task pool
 */

#define SO_STOLE 0
#define SO_BUSY  1
#define SO_NO_WORK 2

#ifndef STEAL_TRYLOCK
#define STEAL_TRYLOCK 1
#endif

#ifndef STEAL_PEEK
#define STEAL_PEEK 1
#endif

#ifndef RAND_INTERVAL
#define RAND_INTERVAL 40
#endif

// No of iterations of waiting after a failed leap
#ifndef BACKOFF_MODE
#define BACKOFF_MODE 50 
#endif

//static Worker *workers;
// cmd arguments pst
//static int
//		n_workers = 0,
//		n_stealable = 0,
//		init_worker_dq_size = INIT_WORKER_DQ_SIZE;

#define NON_USEFUL_INC(_bnc, _tsc) \
	_bnc->non_useful_time += _tsc
#define TASKS_INC(_bnc) \
	_bnc->tasks++
#define STEALS_S_INC(_bnc) \
	_bnc->steals_s++
#define STEALS_F_INC(_bnc) \
	_bnc->steals_f++

static void spin(int n) {
	int i, s = 0;

	// This code should hopefully confuse every optimizer.
	for (i = 1; i <= n; i++) {
		s |= i;
	}
}

static int myrand(unsigned int *seedp, int max) {
	return rand_r(seedp) % max;
}

static int steal(opt_worker_dvs_t self, opt_worker_dvs_t victim, Task *dq_top) {
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_dvs_bench_t bnc = self->bnc;
#endif
	volatile Task *tp;
	void (*f) (Task *, Task *) = NULL; // To suppress a warning!

#if STEAL_PEEK
	tp = victim->dq_bot;
	if (tp->balarm != NOT_STOLEN || tp->f <= T_LAST || !(tp->stealable)) {
#if(PRINT_USEFUL_TIME == 1)
		NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
		return SO_NO_WORK;
	}
#else
	tp = victim->dq_bot;
	if (!(tp->stealable)) {
#if(PRINT_USEFUL_TIME == 1)
		NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
		return SO_NO_WORK;
	}
#endif
	PREFETCH(tp->balarm); // Start getting exclusive access

#if STEAL_TRYLOCK
	if (wool_trylock(victim->dq_lock) != 0) {
#if (PRINT_USEFUL_TIME == 1)
		NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
		return SO_BUSY;
	}
#else
	wool_lock(victim->dq_lock);
#endif
	// now locked!
	tp = victim->dq_bot; // Yes, we need to reread after acquiring lock!

	// The victim might have sync'ed or somebody else might have stolen
	// while we were obtaining the lock;
	// no point in getting exclusive access in that case.
	if (tp->stealable && tp->balarm == NOT_STOLEN && tp->f > T_LAST) {
		tp->balarm = (long unsigned int) self;
		MFENCE;
		f = tp->f;
		if (f > T_LAST) { // Check again after the fence!
			victim->dq_bot++;
		} else {
			tp->balarm = NOT_STOLEN;
			tp = NULL;
		}
	} else {
		tp = NULL; // Already stolen by someone else
	}
	wool_unlock(victim->dq_lock);

	if (tp != NULL) {
#if(PRINT_USEFUL_TIME == 1)
		NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
		f(dq_top, (Task *) tp);
#if(PRINT_USEFUL_TIME == 1)
		tsc = tsc_get;
#endif
		// instead of locking, so that the return value is really updated
		// the return value is already written by the wrapper (f)
		SFENCE;
		tp->balarm = STOLEN_DONE;
#if (PRINT_TASK_STATS == 1)
		TASKS_INC(bnc);
#endif
		return SO_STOLE;
	}
#if(PRINT_USEFUL_TIME == 1)
	NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
	return SO_NO_WORK;
}

void __wool0_dvs_sync(volatile Task *t, balarm_t a) {
	opt_worker_dvs_t self = (opt_worker_dvs_t) wool_get_self(t);
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_dvs_bench_t bnc = self->bnc;
#endif

	wool_lock(self->dq_lock);
	if (a == STOLEN_DONE || t->balarm == STOLEN_DONE) {
		/* Do nothing */
	} else if (a > B_LAST) {
		/* Stolen and in progress; let's leapfrog! */
		int done = 0;
		opt_worker_dvs_t thief = (opt_worker_dvs_t) a;

		wool_unlock(self->dq_lock);

		/* Now leapfrog */

		do {
			uint8_t steal_outcome;
#if(PRINT_USEFUL_TIME == 1)
			NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
			steal_outcome = steal(self, thief, (Task *) t + 1);
#if(PRINT_USEFUL_TIME == 1)
			tsc = tsc_get;
#endif
#if (PRINT_STEAL_STATS == 1)
			if (steal_outcome == SO_STOLE) {
				STEALS_S_INC(bnc);
			} else {
				STEALS_F_INC(bnc);
			}
#endif
			if (steal_outcome != SO_STOLE) {
				spin(BACKOFF_MODE);
			}
			if (t->balarm == STOLEN_DONE) { // Leapfrogging is over!
				done = 1;
			}
		} while (!done);

		wool_lock(self->dq_lock);

	} else {
		fprintf(stderr, "Unknown task state %lu in sync\n", (unsigned long) a);
		exit(1);
	}
	self->dq_bot--;
	t->balarm = NOT_STOLEN;

	wool_unlock(self->dq_lock);
#if(PRINT_USEFUL_TIME == 1)
	NON_USEFUL_INC(bnc,tsc_diff(tsc));
#endif
}

bool __wool0_dvs_work(opt_worker_t s, opt_allotment_t allot) {
	opt_worker_dvs_t self = (opt_worker_dvs_t) s;
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_dvs_bench_t bnc = self->bnc;
#endif
	int attempts = 0;

	coreid_t vid = self->Vid;
	coreid_t i = 0;

	uint8_t steal_outcome;
	opt_worker_dvs_t victim = self;

	while (vid == self->Vid && i < self->V_sz) {
	#if(PRINT_USEFUL_TIME == 1)
		NON_USEFUL_INC(bnc,tsc_diff(tsc));
	#endif
		steal_outcome = steal(self, victim, self->dq_base);
	#if(PRINT_USEFUL_TIME == 1)
		tsc = tsc_get;
	#endif
		attempts++;
	#if (PRINT_STEAL_STATS == 1)
		if (steal_outcome == SO_STOLE) {
			STEALS_S_INC(bnc);
		} else {
			STEALS_F_INC(bnc);
		}
	#endif
		if (steal_outcome == SO_STOLE) {

			// Ok, this is clunky, but the idea is to record if we had to try many times
			// before we managed to steal.
			attempts = 0;
			break;
		}
		victim = self->V[i++];
	}
#if(PRINT_USEFUL_TIME == 1)
	tsc = tsc_diff(tsc);
	NON_USEFUL_INC(bnc,tsc);
#endif
	return true;
}


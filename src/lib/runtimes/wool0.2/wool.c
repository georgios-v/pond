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
#include <bench.h>
#include "types.h"
#include <wool0.h>


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

static int steal(opt_worker_wool0_t self, opt_worker_wool0_t victim, Task *dq_top) {
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_bench_t bnc = self->bnc;
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

void __wool0_sync(volatile Task *t, balarm_t a) {
	opt_worker_wool0_t self = (opt_worker_wool0_t) wool_get_self(t);
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_bench_t bnc = self->bnc;
#endif

	wool_lock(self->dq_lock);
	if (a == STOLEN_DONE || t->balarm == STOLEN_DONE) {
		/* Do nothing */
	} else if (a > B_LAST) {
		/* Stolen and in progress; let's leapfrog! */
		int done = 0;
		opt_worker_wool0_t thief = (opt_worker_wool0_t) a;

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

bool __wool0_work(opt_worker_t s, opt_allotment_t allot) {
	opt_worker_wool0_t self = (opt_worker_wool0_t) s;
#if(PRINT_USEFUL_TIME == 1)
	size_t tsc = tsc_get;
	wool0_bench_t bnc = self->bnc;
#endif
	unsigned int n = allot->count;
	unsigned int self_idx = allot->idx[self->idx];
	int attempts = 0;

	uint8_t steal_outcome;

	int i = self->i;
	coreid_t victim_idx = self->victim_idx;

	// Computing a random number for every steal is too slow, so we do some amount of
	// sequential scanning of the workers and only randomize once in a while, just 
	// to be sure.

	if (i > 0) {
		i--;
		victim_idx++;
		// A couple of if's are faster than a %...
		if (victim_idx == self_idx) victim_idx++;
		if (victim_idx >= n) victim_idx = 0;
	} else {
		i = myrand(&self_idx, RAND_INTERVAL);
		victim_idx = (myrand(&self_idx, n - 1) + self_idx + 1) % n;
	}
	self->victim_idx = victim_idx;
	self->i = i;
	opt_worker_wool0_t victim = (opt_worker_wool0_t) allot->workers[victim_idx];

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
	}
#if(PRINT_USEFUL_TIME == 1)
	tsc = tsc_diff(tsc);
	NON_USEFUL_INC(bnc,tsc);
#endif
	return true;
}


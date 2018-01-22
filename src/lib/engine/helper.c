/*
 * File:   helper.c
 * Author: Georgios Varisteas
 *
 * Created on March 7, 2014, 10:44 AM
 */


#ifdef BARRELFISH
#include <barrelfish/barrelfish.h>
#include <barrelfish/types.h>
#include <barrelfish/threads.h>
#include <barrelfish/waitset.h>
#include <pthread_compat.h>
#endif

#include <sys/unistd.h>
#include <config.h>
#include <util.h>
#include "types.h"
#include "syssched.h"
#include "helper.h"
#include <bench.h>

extern volatile bool __pond_stop_work;	///< Boolean flag to terminate the scheduler. Set to true to shutdown
extern pond_thread_t *_threads;

int helper_init(void *arg);

void helper_dmc(dmc_t dmc, program_t prg) {
	// check if it's an increase or a decrease
	if (dmc->type == DMC_INC) {
#if (PRINT_ISLAND_D_INC == 1)
		cycles_t tsc = tsc_get;
#endif
		int n = sched_allotment_increase(prg, dmc->count);
		if(n > 0) {
			if(prg->vtbl->inc)
				prg->vtbl->inc(prg->opt, prg->allotment);

			coreid_t i;
			for(i = 0; i < prg->allotment->count; i++){
				worker_resume_get(_threads, prg->allotment->workers[i]->idx);
			}
			cprint(PRINT_ISLAND_D_INC, "[%lu] INC(%d): %ld\n", tsc, n, tsc_diff(tsc));
		}
	} else if (dmc->type == DMC_DEC) {
#if (PRINT_ISLAND_D_DEC == 1)
		cycles_t tsc = tsc_get;
#endif
		int n = sched_allotment_decrease(prg, dmc->count);
		if(n > 0) {
			if(prg->vtbl->dec)
				prg->vtbl->dec(prg->opt, prg->allotment);
			cprint(PRINT_ISLAND_D_DEC, "[%lu] DEC(%d): %ld\n", tsc, n, tsc_diff(tsc));
		}
	}
}

static void helper_wrapper(void *arg){
	helper_t me = (helper_t) arg;
	me->state = TRD_PAUSED;
#ifdef BARRELFISH
	// If busy exit
	if(me->state == TRD_PAUSED) {
		cerror(_SYS_DEBUG_HELPER, "Helper sleeping\n");
		return;
	}
#endif
	program_t prg = me->program;
	// make sure to shutdown before accessing freed memory
	// On Barrelfish we use periodic events, that will recall this on
	// [currently] fixed intervals
	// On Linux we have an infinite loop with a sleep function
#ifndef BARRELFISH
	while(1) {
		pthread_mutex_lock(&(me->lock));
		if(__pond_stop_work == true || me->state == TRD_STOPPED)
			goto helper_exit_loop;
helper_sleep:
		pthread_mutex_unlock(&(me->lock));
		cerror(_SYS_DEBUG_HELPER, "Helper sleeping\n");
		SLEEP(me->sleepus);
#endif

		cerror((_SYS_DEBUG_HELPER || _SYS_VERBOSE_HELPER), "Helper awake\n");


		/******************
		 * RUN THE HELPER *
		 ******************/
		me->dmc->type = DMC_NON;
		me->dmc->count = 0;
		pthread_mutex_lock(&(me->lock));
		if(__pond_stop_work == true || me->state == TRD_STOPPED) {
			goto helper_exit_loop;
		} else if(me->state != TRD_ACTIVE) {
#ifdef BARRELFISH
			pthread_mutex_unlock(&(me->lock));
			return;
#else
			goto helper_sleep;
#endif
		}
		prg->vtbl->hlp(prg->opt, prg->allotment, me->dmc);
		pthread_mutex_unlock(&(me->lock));



		if(me->dmc->type != DMC_NON) {
			// store in history
			me->dmc_history[me->dmc_history_sz] = me->dmc->type;
			cprint(_SYS_VERBOSE_HELPER, "Helper got %d\n", me->dmc_history[me->dmc_history_sz]);
			// detect flux
			if(me->dmc_history_sz > 1) {
				// if I detect a FLUX (DEC-INC-DEC) OR
				// this is DEC with a flux before (DEC*-FLUX) disregard the request
				if((me->dmc->type == DMC_DEC
						&& me->dmc_history[me->dmc_history_sz - 1] == DMC_INC
						&& me->dmc_history[me->dmc_history_sz - 2] == DMC_DEC)
						|| (me->dmc->type == DMC_DEC && me->flux == true)) {
					me->flux = true;
				} else {	// not in flux anymore (INC-FLUX)
					me->flux = false;
				}
			}
			if(me->dmc->type != DMC_NON && me->flux == false) {
				pthread_mutex_lock(&(me->lock));
				if(__pond_stop_work == true || me->state == TRD_STOPPED) {
					goto helper_exit_loop;
				} else if(me->state != TRD_ACTIVE) {
#ifdef BARRELFISH
					pthread_mutex_unlock(&(me->lock));
					return;
#else
					goto helper_sleep;
#endif
				}
				helper_dmc(me->dmc, prg);
				pthread_mutex_unlock(&(me->lock));
			}
			me->dmc_history_sz++;
		}
#ifndef BARRELFISH
		cerror(_SYS_DEBUG_HELPER, "Helper loop\n");
	}
#endif
	cerror(_SYS_DEBUG_HELPER, "Helper return\n");
	return;
helper_exit_loop:
	pthread_mutex_unlock(&(me->lock));
	cerror(_SYS_DEBUG_HELPER, "Helper exiting\n");
#ifdef BARRELFISH
	periodic_event_cancel(me->pe);
	free(me->pe);
#endif
	free(me->dmc);
	free(me);
	pthread_exit(NULL);
	return;
}

/**
 * @XXX: Let the helper reset deferred. That way there can be a variable interval!!
 * @param arg
 * @return
 */
int helper_init(void *arg) {
	helper_t me = (helper_t) arg;
#ifndef BARRELFISH
	setaffinity(me->self, me->flux); // flux is not used till now. Hack to get the core value
	me->flux = false;
	cprint(_SYS_VERBOSE_HELPER, "helper of %d with period %lu\n", me->program->pid, me->sleepus.tv_nsec);
	helper_wrapper(me);
#else
	cprint(_SYS_VERBOSE_HELPER, "helper of %d with period %lu\n", me->program->pid, me->sleepus);
	// initialize as paused till work is created
	me->pe = (struct periodic_event *) checked_malloc(sizeof (struct periodic_event));
	struct waitset *ws = get_default_waitset();
	periodic_event_create(me->pe, ws, me->sleepus, MKCLOSURE(helper_wrapper, me));
	event_dispatch(ws);
#endif
	return 0;
}

helper_t helper_create(coreid_t core, program_t prg, size_t sleep) {
	helper_t h = checked_malloc(sizeof(*h));
	// initialize as paused till work is created
	h->program = prg;
	h->state = TRD_INIT;
	h->dmc = checked_malloc(sizeof(*(h->dmc)));	
	h->dmc_history_sz = 0;
	pthread_mutex_init(&(h->lock), NULL);
#ifdef BARRELFISH
	h->flux = false;
	h->sleepus = sleep;
	errval_t err = domain_thread_create_on(core, helper_init, h);
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "failed to spawn helper on core %u\n", core);
	}
	assert(err_is_ok(err));
#else
	h->flux = core; // hack to transfer core for setaffinity
	h->sleepus.tv_sec = 0;
	h->sleepus.tv_nsec = sleep * 1000;
	errval_t err = pthread_create(&(h->self), NULL, (voidp_func)helper_init, h); // BF requires int as return value
	if(err) {
		error("failed to spawn helper on core %u\n", core);
		exit(1);
	}
#endif
	return h;
}

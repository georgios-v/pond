/*
 * File:   master.c
 * Author: Georgios Varisteas
 *
 * Created on April 30, 2012, 11:05 AM
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
#include <bench.h>
#include "types.h"
#include "topology.h"
#include "master.h"


volatile coreid_t __pond_master_core_id;	///< The id of the core running the scheduler
volatile coreid_t __pond_spawned_workers = TOPOLOGY_CORE_COUNT;	///< Used to wait for all workers to spawn
volatile bool __pond_stop_work = false; ///< Boolean flag to terminate the scheduler. Set to true to shutdown

pond_thread_t *_threads; ///< Array of worker descriptor structures. This is allocated to the number of available cores, system reserved cores are excluded. Thus worker id corresponds to core->idx

cond_t all_done_condition; ///< Generic conditional variable to wait completion of threaded event
mutex_t all_done_mutex; ///< Generic mutex lock for editing the above conditional
mutex_t critical_mutex; ///< Generic mutex lock for editing shared data

int worker(void *arg);


#ifdef BARRELFISH
coreid_t __pond_spanned_cores = TOPOLOGY_CORE_COUNT;	///< Used to wait for domain to finish spanning all cores
/**
 * Callback function on domain spanning.
 * @param arg Unused
 * @param reterr Returned status/error code, asserted on OK
 */
static void spanned_cb(void *arg, errval_t reterr) {
	pthread_mutex_lock(&critical_mutex);
	__pond_spanned_cores--;
	cerror(_SYS_DEBUG_SPAN, "nr_spanned_cores = %d\n", __pond_spanned_cores);
	if (__pond_spanned_cores == 0) {
		signal_all_done(&all_done_condition, &all_done_mutex);
	}
	pthread_mutex_unlock(&critical_mutex);
}
/**
 * Span domain to all available cores
 */
static void domain_span_to_all_cores(void) {
	errval_t err;
	// span to all OTHER cores
	cerror(_SYS_DEBUG_SPAN, "spanning to %d cores\n", TOPOLOGY_CORE_COUNT - 1);
	coreid_t curr_core_nr = (__pond_master_core_id + 1) % TOPOLOGY_CORE_COUNT;
	while (curr_core_nr != __pond_master_core_id) {
		// create new dispatcher
		cerror(_SYS_DEBUG_SPAN, "curr_core_nr = %d\n", curr_core_nr);
		err = domain_new_dispatcher(curr_core_nr, spanned_cb, NULL);
		if (err_is_fail(err)) {
			DEBUG_ERR(err, "failed to span domain\n");
		}
		assert(err_is_ok(err));
		// increment counter
		curr_core_nr = (curr_core_nr + 1) % TOPOLOGY_CORE_COUNT;
	}

	cerror(_SYS_DEBUG_SPAN, "ack\n");
	// we need to wait until the domain got spanned to all other cores
	// this means that we need to wait for n-1 callbacks
	if (__pond_spanned_cores != 0)
		wait_on_all_done(&all_done_condition, &all_done_mutex);
}
#endif

/**
 * Spawn a worker thread on each core.
 */
static int64_t worker_spawn_to_all(void) {
	int64_t i;
	cprint(_SYS_VERBOSE_SPAN, "allocate worker threads\n");
	// allocate the workers collection
	_threads = checked_malloc(sizeof (worker_t) * TOPOLOGY_CORE_COUNT);
	for (i = 0; i < TOPOLOGY_CORE_COUNT; i++) {
		// allocate the worker descriptor
		_threads[i] = checked_malloc(sizeof (*(_threads[i])));
		// spawn new worker thread
#ifdef BARRELFISH
		err = domain_thread_create_on(i, worker, (void *) i);
		if (err_is_fail(err)) {
			DEBUG_ERR(err, "failed to spawn worker on core %u\n", i);
		}
#else
		pthread_create(&(_threads[i]->self), NULL, (voidp_func)worker, (void *) i);
#endif
	}
	wait_on_all_done(&all_done_condition, &all_done_mutex);
	return i;
}

/**
 * Start the scheduler. Span the domain to all cores and spawn a worker thread on each.
 */
void runscheduler(void) {
	coreid_t count;
	topology_cores_init();
	cprint(_SYS_VERBOSE_MASTER, "running scheduler\n");
	pthread_mutex_init(&all_done_mutex, NULL);
	pthread_mutex_init(&critical_mutex, NULL);
	pthread_cond_init(&all_done_condition, NULL);
#ifdef BARRELFISH
	// span domain to all available cores
	count = domain_span_to_all_cores();
	cprint(_SYS_VERBOSE_SPAN, "spanned to %u cores!\n", count);
#endif
	// spawn a worker thread on each core
	count = worker_spawn_to_all();
	cprint(_SYS_VERBOSE_SPAWN, "spawned %d workers!\n", count);
}

/**
 * Stop the scheduler. Signal all worker threads to exit and wait till done.
 */
void stopscheduler(void) {
	// set Shutdown flag to true
	__pond_stop_work = true;
	for (coreid_t i = 0; i <= TOPOLOGY_CORE_COUNT; i++) {
		worker_resume_get(_threads, i);
	}
	// NOTE: helpers will auto-shutdown on the next periodic call;
	// wait for all to shutdown
	wait_on_all_done(&all_done_condition, &all_done_mutex);
}

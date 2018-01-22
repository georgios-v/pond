/*
 * File:   util.h
 * Author: Georgios Varisteas
 *
 * Created on May 25, 2012, 1:15 PM
 */

#ifndef POND_UTIL_H
#define	POND_UTIL_H

#ifdef BARRELFISH
#include <barrelfish/domain.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#endif
#include <pthread_compat.h>
#include <config.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define ENTRIES(array)  (sizeof(array) / sizeof(array[0]))

	static inline void *checked_malloc(int len) {
		void *p = malloc(len);
		if (!p) {
#ifdef BARRELFISH
			USER_PANIC("Run out of memory!");
#else
			error("Run out of memory!");
#endif
			exit(1);
		}
		return p;
	}
	
#ifndef BARRELFISH
	int setaffinity(pthread_t thread, unsigned short int c);
	int getaffinity(pthread_t thread);
#endif
	char *itoa(int n, char *s, unsigned int b);
	char *strrev(char *);

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define SLEEP(T) nanosleep(&T, NULL)

	/**
	 * Wrapper function to wait on the conditional variable for a threaded event to complete
	 */
	static inline __attribute__((always_inline)) void signal_all_done(cond_t *c, mutex_t *m) {
		cerror(_SYS_DEBUG_SIGNAL, "signaling all done\n");
		pthread_mutex_lock(m);
		pthread_cond_signal(c);
		pthread_mutex_unlock(m);
		cerror(_SYS_DEBUG_SIGNAL, "signaled all done\n");
	}

	/**
	 * Wrapper function to wait on the conditional variable for a threaded event to complete
	 */
	static inline __attribute__((always_inline)) void wait_on_all_done(cond_t *c, mutex_t *m) {
		cerror(_SYS_DEBUG_SIGNAL, "wait completion on all other cores...\n");
		pthread_mutex_lock(m);
		pthread_cond_wait(c, m);
		pthread_mutex_unlock(m);
		cerror(_SYS_DEBUG_SIGNAL, "signaled.\n");
	}

	/**
	 * Return the worker descriptor of given index
	 * @param _workers The collection_t of worker descriptors
	 * @param indx The index of the worker to return
	 * @return Pointer to the worker thread descriptor of given index
	 */
#define get_worker(_ws, _i) (_ws[_i])

#ifdef BARRRELFISH
	/**
	 * Helper function to get the worker descriptor without knowing the id
	 * @param _workers The collection_t of worker descriptors
	 * @return Pointer to the worker thread descriptor of given index
	 */
#define get_self(_ws) (_ws[disp_get_core_id()])
#endif

	/**
	 * Wrapper function to pause a worker thread.
	 * @param curr_core_nr The id of the worker thread to be paused.
	 */
#define worker_pause(_w)							\
	if (_w->state != TRD_PAUSED){					\
		pthread_mutex_lock(&(_w->lock));			\
		_w->state = TRD_PAUSED;						\
		pthread_cond_wait(&(_w->plock), &(_w->lock));\
		_w->state = TRD_ACTIVE;						\
		pthread_mutex_unlock(&(_w->lock));			\
	}

#define worker_pause_get(_ws,_cid) \
		worker_pause(get_worker(_ws, _cid))

	/**
	 * Wrapper function to resume a paused worker thread. If the thread is not paused this function has no effect other than a slight unnecessary overhead
	 * @param t curr_core_nr The id of the worker thread to be resumed.
	 */
#define worker_resume(_w)					\
	{										\
	pthread_mutex_lock(&(_w->lock));		\
	if (_w->state == TRD_PAUSED){			\
		_w->pause = false;					\
		pthread_cond_signal(&(_w->plock));	\
	}										\
	pthread_mutex_unlock(&(_w->lock));		\
	}

#define worker_resume_get(_ws, _cid) \
	worker_resume(get_worker(_ws, _cid))

#ifdef	__cplusplus
}
#endif

#endif	/* POND_UTIL_H */


/* 
 * File:   types.h
 * Author: Georgios Varisteas
 *
 * Created on den 4 maj 2012, 22:18
 */

#ifndef __INTERNAL_POND_TYPES_H
#define	__INTERNAL_POND_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#ifdef BARRELFISH
#include <barrelfish/barrelfish.h>
#include <barrelfish/threads.h>
#include <barrelfish/deferred.h>
#endif

#include <config.h>
#include <pond_types.h>

#ifdef	__cplusplus
extern "C" {
#endif

	/// Pond Shell Types
	typedef int (*Command)(int argc, char **argv);
	typedef int (*app_t)(opt_worker_t self, int argc, char **argv);

	/// enumeration of possible worker states
	typedef enum __pond_thread_state pond_thread_state_t;
	/// Worker thread descriptor
	typedef struct __pond_thread *pond_thread_t;

	struct __pond_app_arg_t {
		int argc;
		char **argv;
	};

	struct __pond_app {
		const char *name;
		app_t cmd;
		char *vtbl;	// string of single character codes of allowed runtimes. index 0 used as default
		const char *usage;
	};

	struct __pond_rt {
		const char *name;
		rt_vtbl_t vtbl;
		const char code;
	};

	struct __pond_cmd {
		const char *name;
		Command cmd;
		const char *usage;
	};

	/// enumeration of possible worker states
	enum __pond_thread_state {
		TRD_INIT = 0,			///< default, worker is still initializing
		TRD_ACTIVE = 1,			///< worker running normally
		TRD_PAUSED = 2,			///< worker has successfully paused
		TRD_STOPPED = 3,		///< worker has terminated
		TRD_WAIT = 4,			///< worker waiting on external events
	};

#include "program.h"

	/// Worker descriptor
	struct __pond_thread {
		program_init_t init;		///< Holds the initial closure. Used to start a program. When set, program executes, else must be NULL
		coreid_t *programs_hc;		///< Array of hc of this worker from the Source for each running program on this worker. If NULL, not running
		worker_t *programs;			///< Array of program specific worker data for this thread. Does not need to be unset on worker removal.
		pond_thread_state_t state;		///< worker state
		bool pause;					///< command worker to pause by setting to true
		mutex_t lock;				///< mutex lock used when pausing and resuming the worker
		cond_t plock;				///< conditional used to wait a worker
		pthread_t self;			///< pointer to the thread descriptor of the worker thread. Used for pausing
	};


#ifdef	__cplusplus
}
#endif

#endif	/* __INTERNAL_POND_TYPES_H */


/* 
 * File:   helper.h
 * Author: georgios
 *
 * Created on March 7, 2014, 3:13 PM
 */

#ifndef HELPER_H
#define	HELPER_H

#include <pond_types.h>
#include "types.h"
#include "program.h"


#ifdef	__cplusplus
extern "C" {
#endif
	
	typedef struct __pond_helper {
		volatile pond_thread_state_t state;		///< helper state
		dmc_t dmc;
		dmc_type_t dmc_history[MAX_INT8_SIZE];
		uint8_t	dmc_history_sz;
		bool flux;
#ifdef BARRELFISH
		delayus_t sleepus;				///< wake up period. uint64_t
		struct event_closure cl;
		struct periodic_event *pe;
#else
		struct timespec sleepus;
		pthread_t self;
#endif
		program_t program;
		mutex_t lock;
	} *helper_t;

	void helper_dmc(dmc_t dmc, program_t prg);
	helper_t helper_create(coreid_t core, program_t prg, size_t sleep);

#ifdef	__cplusplus
}
#endif

#endif	/* HELPER_H */


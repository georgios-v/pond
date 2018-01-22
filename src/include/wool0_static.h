/* 
 * File:   wool_static.h
 * Author: georgios
 *
 * Created on March 25, 2013, 6:40 PM
 */

#ifndef WOOL_STATIC_H
#define	WOOL_STATIC_H

#include <stdlib.h>
#include <stdio.h>
#include <pond_types.h>

#ifdef	__cplusplus
extern "C" {
#endif

	unsigned int wool0_get_nstealable(coreid_t count, unsigned int start_value);
	void wool0_update_dq_top(Task *q);
	void wool0_task_inc_lcl(Task *q);
	void __wool0_SYNC(volatile Task *t, balarm_t a);
	Task *__wool0_get_top(opt_worker_t self);
	balarm_t __wool0_sync_get_balarm(Task *t);

#ifdef	__cplusplus
}
#endif

#endif	/* WOOL_STATIC_H */


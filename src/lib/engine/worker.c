#include <config.h>
#include <util.h>
#include "types.h"
#include <bench.h>
#include <pthread_compat.h>
#include "topology.h"
#include "program.h"
#include "syssched.h"
#include "bindings.h"
#include "helper.h"

extern volatile coreid_t __pond_spawned_workers;	///< Number of spawned workers

extern volatile bool __pond_stop_work;	///< Boolean flag to terminate the scheduler. Set to true to shutdown

extern pond_thread_t *_threads;			///< Array of worker thread descriptor structures. This is allocated to the number of available cores, although the master core never spawns a worker, so the corresponding index will always be NULL.

extern cond_t all_done_condition;		///< Generic conditional variable to wait completion of threaded event
extern mutex_t all_done_mutex;			///< Generic mutex lock for editing the above conditional
extern mutex_t critical_mutex;			///< Generic mutex lock for editing shared data

extern mutex_t print_mutex;					///< Generic mutex lock for non-interleaved printing

// defined in pond.c
void shell_cmd_finished(void);

int worker(void *arg);

#if (BENCHMARKING == 1)
static coreid_t print_stats(program_t prg) {
	cerror(_SYS_DEBUG_BENCH, "DEBUG_BENCH: start %d benches, for %d\n", prg->count, prg->pid);
	coreid_t i, n = 0;
#define WORKER_STAT_BUFFER_SZ 1000
	char str[WORKER_STAT_BUFFER_SZ*prg->count];
	char buf[WORKER_STAT_BUFFER_SZ];
	str[0] = '\0';
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++) {
		if(prg->workers[i] != NULL) {
			worker_bench_t b = prg->workers[i]->bench;
			if(b != NULL) {
				stat_print_bench(buf, i, b, prg->workers[i]->program->vtbl->bnc, prg->workers[i]->opt);
				strcat(str, buf);
			}
		}
	}
	print("STATS:\n%s\n", str);
	cerror(_SYS_DEBUG_BENCH, "DEBUG_BENCH: finished.\n");
	return n;
}
#endif

/**
 * Worker thread self-loop function.
 * @param arg The worker id
 * @return 1
 */
int worker(void *arg) {
	// cast the argument to my thread/worker id
	coreid_t widx = (coreid_t) ((uint64_t) arg);
	// cast the argument to my physical core id
	coreid_t curr_core_nr = topology_get_core_from_idx(widx)->id;
	
	// get and store the worker descriptor
	pond_thread_t me = _threads[widx];
	// Pin thread. On Barrelfish this is done on thread_create
#ifndef BARRELFISH
	setaffinity(me->self, curr_core_nr);
#endif
#ifndef BARRELFISH
	cprint(_SYS_VERBOSE_WORKER, "Hello I'm worker %d on %d\n", widx, getaffinity(me->self));
#else
	cprint(_SYS_VERBOSE_WORKER, "Hello I'm worker %d on %d\n", widx, curr_core_nr);
#endif
	// set correct startup values
	me->state = TRD_INIT;
	// initialize
	me->init = (program_init_t) checked_malloc(sizeof (struct __pond_program_init));
	me->init->active = false;
	// pre-allocate the programs
	coreid_t i = 0;
	// pre-allocate the islands, one per program (worker-specific data)
	me->programs = checked_malloc(sizeof(program_t) * (MAX_PROGRAMS));
	me->programs_hc = checked_malloc(sizeof (coreid_t) * (MAX_PROGRAMS));
	for(i = 0; i < MAX_PROGRAMS; i++) {
		me->programs[i] = NULL;
		me->programs_hc[i] = 0;
	}
	pthread_mutex_init(&(me->lock), NULL);
	pthread_cond_init(&(me->plock), NULL);
	// worker init done, notify master of last worker to initialize
	pthread_mutex_lock(&critical_mutex);
	if(--__pond_spawned_workers == 0) {
		signal_all_done(&all_done_condition, &all_done_mutex);
	}
	cerror(_SYS_DEBUG_WORKER, "DEBUG_WORKER: nr_spawned_workers = %u\n", __pond_spawned_workers);
	pthread_mutex_unlock(&critical_mutex);
	cerror(_SYS_DEBUG_WORKER, "DEBUG_WORKER: worker running\n");
	// initialization finished
	// cleanup
	// initialize as paused till work is created
	me->pause = true;
	program_t prg = NULL;
	coreid_t prg_id = MAX_PROGRAMS;
	coreid_t prg_hc = 127;
	coreid_t prg_d = 127;
	// wait for work
	while (__pond_stop_work == false) {
		coreid_t count = 0;
		// check to pause
		if (me->pause) { // || !(me->program_count)) {
			cprint(_SYS_VERBOSE_WORKER, "pausing worker %d\n", curr_core_nr);
			worker_pause(me);
			cprint(_SYS_VERBOSE_WORKER, "resumed worker %d\n", curr_core_nr);
		}
		// check to stop
		if (__pond_stop_work)
			break;
		// check for main()
		if (me->init->active) {
			cerror(_SYS_DEBUG_WORKER, "DEBUG_WORKER: initiating program\n");
			prg = me->init->program;
			// deactivate program init to avoid re-running the program on re-entry
			me->init->active = false;
			// program_init is cleaned up by init before running src
			// so keep shortcuts
			app_t src = me->init->src;
			int argc = me->init->argc;
			char **argv = me->init->argv;
			int argv_offset = me->init->argv_offset;
#if (PRINT_EXC_TIME == 1)
			cycles_t tsc_bgn = tsc_get;
			cprint(BENCHMARKING && _SYS_VERBOSE_WORKER, " [%d] ** core %d, D %d\n", prg->pid, prg->source_idx, prg->count);
#endif
			// optional runtime specific initialization
			if(prg->vtbl->init)
				prg->vtbl->init(prg->opt, prg->allotment);
			prg->active = true;

			for(i = 0; i < prg->allotment->count; i++){
				cprint(_SYS_DEBUG_WORKER, "resuming %d: %d\n", i, prg->allotment->workers[i]->idx);
				worker_resume_get(_threads, prg->allotment->workers[i]->idx);
			}
			// setup helper and resume workers
#if (PRINT_EXC_TIME == 1)
			cycles_t tsc_exec = tsc_get;
			pthread_mutex_lock(&print_mutex);
			cprint(BENCHMARKING, " [%d] EXEC on %d\n", prg->pid, me->programs[prg->pid]->widx);
			pthread_mutex_unlock(&print_mutex);
#endif
			if(prg->helper) {
				prg->helper->state = TRD_ACTIVE;
			}
			
			

			/** call the program **/
			src(me->programs[prg->pid]->opt, argc, &argv[argv_offset]);

	

#if (PRINT_EXC_TIME == 1)
			cycles_t tsc_end = tsc_get;
#endif

			// at this point the taskset is finished, release
			me->programs[prg->pid]->active = prg->active = false;
			
			// label inactive to disable the helper
			if(prg->helper) {
				pthread_mutex_lock(&(prg->helper->lock));
				prg->helper->state = TRD_STOPPED;
				pthread_mutex_unlock(&(prg->helper->lock));
			}

			// start finalization
			// first make sure all workers are down
			cprint(_SYS_DEBUG_WORKER, "********** FINI1 ************\n");
			program_fini(prg);
			cprint(_SYS_DEBUG_WORKER, "********** FINI2 ************\n");
			// now start freeing runtime-set memory
			if(prg->vtbl->fini)
				prg->vtbl->fini(prg->opt, prg->allotment);


			pthread_mutex_lock(&print_mutex);
#if (PRINT_EXC_TIME == 1)
			cycles_t tsc_exec_all = tsc_end - tsc_bgn;
			cycles_t tsc_exec_from_init = tsc_end - tsc_exec;
			print(" [%d] END D %d\n", prg->pid, prg->count);
			print("\n [%d] START,%lu,FINISH,%lu\nTIME,%ld,%ld,%ld,%ld\n", prg->pid,
					tsc_bgn,
					tsc_end,
					tsc_exec_all, bench_tsc_to_ms(tsc_exec_all),
					tsc_exec_from_init, bench_tsc_to_ms(tsc_exec_from_init));
#if (BENCHMARKING == 1)
			stat_total_add(me->programs[prg->pid]->bench, tsc_exec_from_init);
			cerror(_SYS_DEBUG_BENCH, "DEBUG_BENCH: print bench\n");
			print_stats(prg);
#endif
#endif
			print("**PRG[%d]_END**\n", prg->pid);
			pthread_mutex_unlock(&print_mutex);
			// terminate program
#if (PRINT_ISLAND_KILL == 1)
			tsc_end = tsc_get;
#endif
			/** @FIXME: Wait for workers to shutdown before calling kil(), to avoid sync errors (i.e. stealing from already inactive/freed workers).
			 * For now kil() busy-waits but that doesn't solve inter-worker issues */
			sched_allotment_release(prg);
			program_release(prg);
			cprint(PRINT_ISLAND_KILL, "[%lu] KILL: %ld\n", tsc_end, tsc_diff(tsc_end));
			// release pond for progressing cmd execution
			if(me->init->wait)
				shell_cmd_finished();
		} else {
			cprint(_SYS_DEBUG_WORKER, "**** 1 ****\n");
			/** SELECT program to run
			 * the algorithm schedules on two criteria, program's D and current
			 * worker's hc from program's S
			 * it will schedule the program with smallest D for which the worker
			 * is closer to S
			 * each time a program is selected, store its D and hc
			 * every consecutive time a program is selected, increase its
			 * buffered D
			 * thus ensuring that all will be selected.
			 * once program A's bufferred d grows beyond the program B's d,
			 * B is selected
			 * Next time A compares with its true value of D, NOT the previously
			 * buffered, thus gets reselected.
			 * That way more weight is given to high priority programs, without
			 * negating the schedulability of the rest
			 */
			/** @TODO: deal with D changing between iterations */

			// check that the previous program is still running
			// it can happen for a new to start between iterations, 
			// so that this worker missed the old being inactive
			if(prg_id < MAX_PROGRAMS && me->programs[prg_id] != NULL && prg != me->programs[prg_id]->program) {
				prg = NULL;
				prg_id = MAX_PROGRAMS;
				prg_hc = 127;
				prg_d = 127;
			}

			// iterate non-null entries of the programs array
			// schedule the island with smallest D for which I'm closer to S
			count = i = 0;
			do {
			cprint(_SYS_DEBUG_WORKER, "**** 1a %d %p ****\n", i, me->programs[i]);
				// Vet i-th program. Must be active and not the previous one.
				if(me->programs[i] != NULL && me->programs[i]->program->active == true) {
			cprint(_SYS_DEBUG_WORKER, "**** 1b %d ****\n", i);
					// Compare to the previous one, if any and active
					if(i != prg_id && (prg == NULL || (prg != NULL && prg->active == false) ||
							(me->programs[i]->program->count <= prg->count && me->programs_hc[i] < prg_hc))) {
			cprint(_SYS_DEBUG_WORKER, "**** 1c %p %p ****\n", prg, me->programs[i]->program);
						prg_id = i;
						prg_hc = me->programs_hc[i];
						prg_d = me->programs[i]->program->count;
						prg = me->programs[i]->program;
					}
					count++;
				}
			} while (++i < MAX_PROGRAMS);
			cprint(_SYS_DEBUG_WORKER, "**** 2 %d ****\n", count);
			cerror(_SYS_DEBUG_WORKER, "DEBUG_WORKER: stealing from %d\n", prg_id);
			// start stealing
			// check:
			// if the program is still active for the worker
			// if a program was actually set to be scheduled
			// if the program is active in general
			if(prg != NULL && prg->active == true && me->programs[prg_id] != NULL) {
				// this returns the status of the island on this worker
				// if false, worker has been removed from the island
				// deactivate and reset
#if(PRINT_EXC_TIME == 1)
				cycles_t tsc_bgn = tsc_get;
#endif
				cprint(_SYS_DEBUG_WORKER, "**** 3 ****\n");
				
				
				
				prg->vtbl->work(me->programs[prg_id]->opt, prg->allotment);
				
				prg_d++;
				prg_hc++;

//				else {
//					me->programs[prg_id] = NULL;
//					prg_id = MAX_PROGRAMS;
//					prg_hc = 127;
//					prg_d = 127;
//					prg = NULL;
//				}
#if(PRINT_EXC_TIME == 1)
				cycles_t tsc_end = tsc_get;
				stat_total_add(me->programs[prg_id]->bench, tsc_end - tsc_bgn);
				stat_system_stop(me->programs[prg_id]->bench, tsc_bgn);
				stat_system_start(me->programs[prg_id]->bench, tsc_end);
				cprint(_SYS_DEBUG_WORKER, "STAT: %ld\n", tsc_end - tsc_bgn);
#endif
				if(prg->active == false)
					me->programs[prg_id]->active = false;
			}
//			else if(prg != NULL && prg->active == false && me->programs[prg_id] != NULL) {
//				me->programs[prg_id]->active = false;
//			}
			cprint(_SYS_DEBUG_WORKER, "**** 4 ****\n");
		}
		// pause if no programs active on me
		me->pause = !(count);
		cprint(_SYS_DEBUG_WORKER && count == 0, "**** 5 %d %d ****\n", me->pause, count);
	}
	// label worker as stopped
	me->state = TRD_STOPPED;
	// shutting down
	pthread_mutex_lock(&critical_mutex);
	// notify master of last worker shutdown
	if (--__pond_spawned_workers == 0) {
		signal_all_done(&all_done_condition, &all_done_mutex);
	}
	pthread_mutex_unlock(&critical_mutex);
	// ready to terminate
	cprint(_SYS_VERBOSE_WORKER, "thread exiting\n");
	// cleanup
	free(me->programs);
	free(me->programs_hc);
	// exit
	pthread_exit(NULL);
	return 0;
}

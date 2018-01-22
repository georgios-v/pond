/*
 * File:   config.h
 * Author: Georgios Varisteas
 *
 * Created on May 7, 2012, 9:57 AM
 */

#ifndef POND_CONFIG_H
#define	POND_CONFIG_H

#ifdef BARRELFISH
#include <barrelfish/types.h>
#include <barrelfish/barrelfish.h>
#endif

// Execution measurements

///< Central switch to (de)activate benchmarking
///< deactivating this will break PondP, scheduler.c:~170

#ifndef BENCHMARKING
#define BENCHMARKING 1
#endif

#if (BENCHMARKING == 1)
///< Print stats in analytical form with labels, hard to parse but easy to read
#ifndef PRINT_ANALYTICAL
#define PRINT_ANALYTICAL 0
#endif
///< Print total taskset execution time
#ifndef PRINT_EXC_TIME
#define PRINT_EXC_TIME 1
#endif
///< Print spawn time per task spawned.
#ifndef PRINT_SPN_TIME
#define PRINT_SPN_TIME 0
#endif
///< Print sync time per task synced.
#ifndef PRINT_SNC_TIME
#define PRINT_SNC_TIME 0
#endif
///< Print steal time per worker's steal.
#ifndef PRINT_STL_TIME
#define PRINT_STL_TIME 0
#endif
///< Print steal time per worker's steal.
#ifndef PRINT_USEFUL_TIME
#define PRINT_USEFUL_TIME 1
#endif
///< Aggregate timing measurements
#ifndef PRINT_AGGREGATES
#define PRINT_AGGREGATES 0
#endif
///< Values to keep for computing the avg. When limit is reached, average is computed and stored as first entry, to fill up the array again.
#ifndef PRINT_AGGREGATES_SZ
#define PRINT_AGGREGATES_SZ 30
#endif
///< Print each measurement when taken. Dangerous!
#ifndef PRINT_SINGLES
#define PRINT_SINGLES 0
#endif
///< Print variance on aggregate values
#ifndef PRINT_VARIANCE
#define PRINT_VARIANCE 1
#endif
///< Print statistics on successful and failed steal attempts, per worker.
#ifndef PRINT_STEAL_STATS
#define PRINT_STEAL_STATS 1
#endif
///< Print statistics on amount of tasks executed per worker.
#ifndef PRINT_TASK_STATS
#define PRINT_TASK_STATS 1
#endif
///< Print the current per worker number of spawned tasks
#ifndef PRINT_TASK_RATE
#define PRINT_TASK_RATE 0
#endif
///< Print the task_rate per worker, on every DMC change
#ifndef PRINT_TASK_RATE_ON_DMC
#define PRINT_TASK_RATE_ON_DMC 0
#endif
///< Print the time of the first task-spawn per worker.
#ifndef PRINT_FIRST_SPAWN
#define PRINT_FIRST_SPAWN 1
#endif
#if (PRINT_STEAL_STATS == 1)
///< Print the time of the first successful steal per worker.
#ifndef PRINT_FIRST_STEAL
#define PRINT_FIRST_STEAL 1
#endif
#endif
///< Print the time to create an island
#ifndef PRINT_ISLAND_CREATE
#define PRINT_ISLAND_CREATE 1
#endif
///< Print the time to increase an island's size
#ifndef PRINT_ISLAND_D_INC
#define PRINT_ISLAND_D_INC 1
#endif
///< Print the time to decrease an island's size
#ifndef PRINT_ISLAND_D_DEC
#define PRINT_ISLAND_D_DEC 1
#endif
///< Print the time to kill an island
#ifndef PRINT_ISLAND_KILL
#define PRINT_ISLAND_KILL 1
#endif
///< Execute the list() pond function, after each successful DMC
#ifndef PRINT_LIST_ON_DMC
#define PRINT_LIST_ON_DMC 0
#endif
#endif



// System configuration
///< set to 0 to globally supress debug output
#ifndef _SYS_DEBUG_GLOBAL
#define _SYS_DEBUG_GLOBAL 0
#endif

#ifdef _SYS_DEBUG_GLOBAL
#ifndef _SYS_DEBUG_SCHED
#define _SYS_DEBUG_SCHED _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_MAIN
#define _SYS_DEBUG_MAIN _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_SHELL
#define _SYS_DEBUG_SHELL _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_SIGNAL
#define _SYS_DEBUG_SIGNAL _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_SPAN
#define _SYS_DEBUG_SPAN _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_HELPER
#define _SYS_DEBUG_HELPER _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_MASTER
#define _SYS_DEBUG_MASTER _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_SPAWN
#define _SYS_DEBUG_SPAWN _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_SYNC
#define _SYS_DEBUG_SYNC _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_WRAP
#define _SYS_DEBUG_WRAP _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_FOR
#define _SYS_DEBUG_FOR _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_WORKER
#define _SYS_DEBUG_WORKER _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_STEAL
#define _SYS_DEBUG_STEAL _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_CREATE
#define _SYS_DEBUG_CREATE _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_INC
#define _SYS_DEBUG_INC _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_DEC
#define _SYS_DEBUG_DEC _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_KILL
#define _SYS_DEBUG_KILL _SYS_DEBUG_GLOBAL
#endif
#ifndef _SYS_DEBUG_BENCH
#define _SYS_DEBUG_BENCH _SYS_DEBUG_GLOBAL
#endif
#endif

///< set to 0 to globally supress verbose output
#ifndef _SYS_VERBOSE_GLOBAL
#define _SYS_VERBOSE_GLOBAL 0
#endif

#ifdef _SYS_VERBOSE_GLOBAL
#ifndef _SYS_VERBOSE_MAIN
#define _SYS_VERBOSE_MAIN _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SCHED
#define _SYS_VERBOSE_SCHED _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SHELL
#define _SYS_VERBOSE_SHELL _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SIGNAL
#define _SYS_VERBOSE_SIGNAL _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SPAN
#define _SYS_VERBOSE_SPAN _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_HELPER
#define _SYS_VERBOSE_HELPER 0
#endif
#ifndef _SYS_VERBOSE_MASTER
#define _SYS_VERBOSE_MASTER _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SPAWN
#define _SYS_VERBOSE_SPAWN _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_SYNC
#define _SYS_VERBOSE_SYNC _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_WRAP
#define _SYS_VERBOSE_WRAP _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_FOR
#define _SYS_VERBOSE_FOR _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_WORKER
#define _SYS_VERBOSE_WORKER _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_STEAL
#define _SYS_VERBOSE_STEAL _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_CREATE
#define _SYS_VERBOSE_CREATE _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_INC
#define _SYS_VERBOSE_INC _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_DEC
#define _SYS_VERBOSE_DEC _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_KILL
#define _SYS_VERBOSE_KILL _SYS_VERBOSE_GLOBAL
#endif
#ifndef _SYS_VERBOSE_BENCH
#define _SYS_VERBOSE_BENCH _SYS_VERBOSE_GLOBAL
#endif
#endif


///< The default wake up period for the helpers to checkup on a task-set's workload
#ifndef HELPER_SLEEP_TIME
#define HELPER_SLEEP_TIME 50000
#endif
#ifndef HELPER_WAIT_TIME
#define HELPER_WAIT_TIME 5000000
#endif

#ifndef MAX_PROGRAMS
#define MAX_PROGRAMS 5
#endif
///< default diaspora value for new task-sets. This times 2 times TOPOLOGY_DIMENSIONS plus 1 equals the island's size.
#ifndef TASKSET_INIT_D
#define TASKSET_INIT_D 2
#endif
///< number of island zones to allow overlapping of. For example, if set to 1 then only the outermost zone of island X can be overlapped with the outermost zone of island Y.
#ifndef ZONE_OVERLAPPING
#define ZONE_OVERLAPPING 1
#endif
///< this is another restriction on zone overlapping, denoting the maximum number of islands per worker.
#ifndef ISLANDS_PER_WORKER
#define ISLANDS_PER_WORKER 1
#endif
///< maximum number of tasksests for which each worker can be the source. This times #cores gives a upper bound on the number of simultaneus task-sets in the system.
#ifndef MAX_TASKSETS_PER_WORKER
#define MAX_TASKSETS_PER_WORKER 1
#endif
///< the islands array of each worker is fixed to this size. each taskset gets an id from 0 to 15 and has a predefined slot in the array of any worker. ids are repeated
#ifndef MAX_ISLANDS
#define MAX_ISLANDS (MAX_TASKSETS_PER_WORKER * MAX_PROGRAMS)
#endif
///< just an alias so that staff make sense in different contexts.
#ifndef MAX_TASKSETS
#define MAX_TASKSETS MAX_ISLANDS
#endif

///< a guestimate limit value for looping the victims set versions
#ifndef MAX_VERSION
#define MAX_VERSION 125
#endif
///< maximum character per line in the shell
#ifndef MAX_LINE
#define MAX_LINE 512
#endif

#include "definitions.h"

#ifdef BARRELFISH
#include <barrelfish/debug.h>
#endif







#endif	/* POND_CONFIG_H */


/*
 * File:   pond_types.h
 * Author: Georgios Varisteas
 *
 * Created on March 7, 2014, 9:29 AM
 */

#ifndef POND_TYPES_H
#define	POND_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef BARRELFISH
#include <barrelfish/types.h>
#include <barrelfish/barrelfish.h>
#include <barrelfish/threads.h>
#include <barrelfish/waitset.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
	typedef size_t			cycles_t;
	typedef unsigned long	errval_t;
	typedef uint16_t coreid_t;
#endif
#include <config.h>
#include <pthread_compat.h>


#define APP_ARGS	opt_worker_t self, int argc, char **argv

#define MAX_INT8_SIZE 255

	typedef enum __pond_dmc_type {
		DMC_NON = 0,
		DMC_INC = 1,
		DMC_DEC = 2,
		DMC_KIL = 3
	} dmc_type_t;

	typedef struct __pond_dmc {
		coreid_t count;
		dmc_type_t type;
	} *dmc_t;

	typedef void *opt_program_t;

	typedef struct opt_worker {
		coreid_t idx;				///< holds the actual worker idx (from worker_t) used exclusively in program_worker_let
	} *opt_worker_t;				///< Worker specific program options. Set by the runtime by each worker upon its init, used by the helper

	/** Allotment wide common program options. 
	 * Populated by pond and used by the runtime. 
	 * Every runtime-defined subtype must contain at least these fields in this order */
	typedef struct opt_allotment {
		uint8_t pid;
		opt_worker_t *workers;	///< List of runtime-specific worker descriptor structures
		coreid_t *idx;			///< Maps a worker's idx (w->idx) to its dynamic index in the allotment (w = workers[idx[w->idx]]).
		coreid_t count;			///< Current allotment size
	} *opt_allotment_t;

	typedef void (*void_func)(void);
	typedef void * (*voidp_func)(void *);
	typedef int (*cmd_func)(int argc, char *argv[]);

	typedef opt_program_t (*rt_init_p_func)(coreid_t cores);
	typedef opt_worker_t (*rt_init_w_func)(opt_program_t opt, coreid_t idx);
	typedef void (*rt_init_func)(opt_program_t opt, opt_allotment_t allot);
	typedef bool (*rt_work_func)(opt_worker_t s, opt_allotment_t allot);
	typedef void (*rt_dmc_func)(opt_program_t opt, opt_allotment_t allot);
	typedef bool (*rt_kil_func)(opt_program_t opt);
	typedef bool (*rt_kil_w_func)(opt_worker_t opt);
	typedef void (*rt_dfrd_func)(opt_program_t opt, opt_allotment_t allot, dmc_t dmc);
	typedef size_t (*rt_bnc_func)(char *buffer, opt_worker_t w);


	typedef struct __runtime_vtbl *rt_vtbl_t;
	struct __runtime_vtbl {
		rt_init_func init;
		rt_init_p_func init_p;
		rt_init_w_func init_w;
		rt_init_func fini;
		rt_work_func work;
		rt_dmc_func inc;
		rt_dmc_func dec;
		rt_kil_func kil;
		rt_kil_w_func kil_w;
		rt_dfrd_func hlp;
		rt_bnc_func bnc;
	};


#ifdef	__cplusplus
}
#endif

#endif	/* POND_TYPES_H */



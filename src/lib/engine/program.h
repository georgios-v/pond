/* 
 * File:   program.h
 * Author: Georgios Varisteas
 *
 * Created on March 11, 2014, 12:25 PM
 */

#ifndef PROGRAM_H
#define	PROGRAM_H

#include "types.h"
#include <bench.h>
#include "bitmap.h"

#ifdef	__cplusplus
extern "C" {
#endif

	/// Allotted worker thread descriptor, per-program data
	typedef struct __pond_worker *worker_t;		///< generic placeholder for island structure, to be set & typecast by the programming model
	/// Running program descriptor
	typedef struct __pond_program *program_t;	///< Allotment wide internal program options. Set by pond, never seen by the runtime

	typedef struct __pond_program_init *program_init_t;

	struct __pond_worker {
		volatile bool active;	///< per worker activation toggle
		uint8_t widx;			///< worker thread id
#if(BENCHMARKING == 1)
		worker_bench_t bench;
#endif
		coreid_t hc;			///< Distance of this worker to the source
		// implementation specific fields
		program_t program;		///< pond-set program options
		opt_worker_t opt;		///< runtime-set worker specific options
	};

#include "helper.h"

	struct __pond_program {
		volatile bool active;		///< Progream wide activation toggle. Set to false to terminate the program.
		uint8_t pid;				///< Program numerical identifier. Unique at any point but not in history. Replaces process id.
		coreid_t source_idx;		///< Source core
		worker_t *workers;			///< Array of ALL workers ever allotted to the program
		coreid_t count;				///< Amount of ALL allotted workers
		opt_allotment_t allotment;	///< Public structure of currently allotted runtime-allocated worker descriptors
		rt_vtbl_t vtbl;				///< function table for the runtime
		opt_program_t opt;			///< pointer to the runtime-set options structure
		helper_t helper;
	};

	struct __pond_program_init {
		volatile bool active;
		app_t src;					///< the main function of the taskset to be called by the source
		int argc;
		char **argv;
		int argv_offset;
		bool wait;
		program_t program;
	};
	
	program_t program_init(coreid_t source_idx, rt_vtbl_t vtbl);
	void program_fini(program_t prg);
	void program_release(program_t prg);

	worker_t program_worker_set(program_t prg, coreid_t idx, coreid_t hc);
	worker_t program_worker_let(program_t prg, coreid_t idx);

#ifdef	__cplusplus
}
#endif

#endif	/* PROGRAM_H */


/*
 * File:   types.h
 * Author: Georgios Varisteas
 *
 * Created on March 10, 2014, 1:17 PM
 */

#ifndef WOOL0_ASTEAL_TYPES_H
#define	WOOL0_ASTEAL_TYPES_H

#include <config.h>
#include "../types.h"
#include <wool0.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define ASTEAL_DELTA 8
#define ASTEAL_RESP 1.5

	typedef struct wool0_bench *wool0_asteal_bench_t;

	typedef struct opt_program_wool0_asteal {
		cycles_t L;
		int d;
		unsigned int stealable;
		cycles_t *non_stealable;
	} *opt_program_wool0_asteal_t;

#ifdef	__cplusplus
}
#endif

#endif	/* WOOL0_ASTEAL_TYPES_H */


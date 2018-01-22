/*
 * File:   syssched.h
 * Author: Georgios Varisteas
 *
 * Created on February 7, 2014, 11:13 AM
 */

#ifndef SYSSCHED_H
#define	SYSSCHED_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Populate a buffer of size count with candidate cores for an allotment increase
 * @param buffer	The buffer to populate. Must be pre-allocated
 * @param source	The source core of the program to increase its allotment
 * @param count		The number of cores to find
 * @return			The number of cores found
 */
coreid_t sched_allotment_increase(program_t prg, coreid_t count);

/**
 * Populate a buffer of cores to release from an allotment based on distance
 * @param buffer	The pre-allocated buffer to populate
 * @param source	The source of the allotment to decrement
 * @param count		The number of cores to find
 * @return			The number of cores found
 */
coreid_t sched_allotment_decrease(program_t prg, coreid_t count);

/**
 * Create a new allotment, including the cores in a buffer.
 * If source points to NULL, a source is found using distance based algorithm
 * If buffer is NULL use distance algorithm to populate it
 * @param prg		Pointer placeholder for a program descriptor. If not NULL, settings forced
 * @param source_id	The physical id of the core to use as source. If settings forced, should mirror the value in prg
 * @param count		The number of cores to allot, if settings forced should mirror the value in prg
 * @param vtbl		The function table for the chosen runtime. To be used for invoking runtime-specific initialization functions
 * @return			The number of new cores allotted, including the source
 */
coreid_t sched_allotment_create(program_t *prg, int source_id, coreid_t count, rt_vtbl_t vtbl);

/**
 * Release an allotment completely.
 * @param source	Pointer to the source core for the program whose allotment is released
 */
void sched_allotment_release(program_t prg);


#ifdef	__cplusplus
}
#endif

#endif	/* SYSSCHED_H */


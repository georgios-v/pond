/*
 * File:   syssched.c
 * Author: Georgios Varisteas
 *
 * Created on June 20, 2013, 9:38 AM
 */

#include <config.h>
#include <util.h>
//#include "types.h"

#include "topology.h"
#include "types.h"
#include "program.h"
#include "syssched.h"
#include "bitmap.h"


#ifndef _SYS_DEBUG_SCHED
#define _SYS_DEBUG_SCHED 0
#endif
#ifndef _SYS_VERBOSE_SCHED
#define _SYS_VERBOSE_SCHED 1
#endif

extern core_t topology_cores[];			///< all available cores

/**
 * Calculate next source core, using the distance-based algorithm.
 * This function only picks the best-candidate core; it does not change any data
 * @return returns the selected core object.
 */
core_t *sched_core_source_get(void);
/**
 * Mark a core as source
 * @param source	The core being marked as source
 */
void sched_core_source(core_t *source);
/**
 * Unmark a core as source
 * @param source	The core being unmarked as source
 */
void sched_core_unsource(core_t *source);
/**
 * Mark a core as assigned to a program
 * @param idx		The index of the core being marked
 */
static inline void sched_core_allot(coreid_t idx);
/**
 * Unmark a core as assigned to a program
 * @param idx		The index of the core being unmarked
 */
static inline void sched_core_release(coreid_t idx);

static bool sched_is_core_allotted(program_t prg, coreid_t idx) {
	if(prg->workers[idx] != NULL)
		return prg->allotment->idx[idx] - TOPOLOGY_CORE_COUNT;
	return 0;
}

core_t *sched_core_source_get(void){
	// for each core in cores
	// calculate its inverse distance sum from each source in sources
	// return the smallest value
	coreid_t i,j;
	core_t *non_idle[TOPOLOGY_CORE_COUNT];
	coreid_t non_idle_count = 0;
	// first find all sources
	coreid_t sources[TOPOLOGY_CORE_COUNT];
	coreid_t sources_count = 0;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++){
		if(topology_cores[i].sourced)
			sources[sources_count++] = i;
	}
	// weight cannot exceed in value the number of cores
	float weight_min = TOPOLOGY_CORE_COUNT;
	// this is the pointer to the calculated new source
	core_t *new_source = NULL;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++){
		core_t *c = &topology_cores[i];
		pthread_mutex_lock(&(c->lock));
		float weight = 0;
		float weight_a = 0;
		float weight_p = 1;
		if(c->sourced) {
			pthread_mutex_unlock(&(c->lock));
			continue;
		}
		if(c->assignments > 0) {
			// non-idle core, keep for later
			non_idle[non_idle_count++] = c;
			continue;
		}
		for(j = 0; j < sources_count; j++){
			// get distance between cores[i] and sources[j]
			coreid_t hc = topology_get_distance_of_cores(c->idx, sources[j]);
			weight_a = weight_a*hc + weight_p;
			weight_p = weight_p*hc;
		}
		// get the final weight
		weight = (1.0*weight_a)/weight_p;
		// compare to current minimum
		if(weight < weight_min){
			weight_min = weight;
			new_source = c;
		}
		pthread_mutex_unlock(&(c->lock));
	}
	if(new_source != NULL) {
		for(i = 0; i < non_idle_count; i++){
			pthread_mutex_unlock(&(non_idle[i]->lock));
		}
		return new_source;
	}
	// if the above failed retry the non-idle cores
	for(i = 0; i < non_idle_count; i++){
		core_t *c = non_idle[i];
		float weight = 0;
		int weight_a = 0;
		int weight_p = 0;
		for(j = 0; j < sources_count; j++){
			// get distance between cores[i] and sources[j]
			coreid_t hc = topology_get_distance_of_cores(c->idx, sources[j]);
			weight_a = weight_a*hc + weight_p;
			weight_p = weight_p*hc;
		}
		// get the final weight
		weight = (1.0*weight_a)/weight_p;
		// compare to current minimum
		if(weight < weight_min){
			weight_min = weight;
			new_source = c;
		}
		pthread_mutex_unlock(&(c->lock));
	}
	return new_source;
}

/**
 * Populate a buffer of size count with candidate cores for an allotment increase
 * @param buffer	The buffer to populate. Must be pre-allocated
 * @param source	The source core of the program to increase its allotment
 * @param count		The number of cores to find
 * @return			The number of cores found
 */
coreid_t sched_allotment_increase(program_t prg, coreid_t count) {
	if(count == 0)
		return 0;
	cprint(_SYS_VERBOSE_SCHED, "SCHED: Increasing allotment by %d\n", count);
	// buffer to hold distance-viable cores that are already assigned
	core_t *assigned[count];
	coreid_t hc[count];
	coreid_t assigned_idx = 0;
	// iterate the source's distances vectors to find viable cores
	coreid_t i,j, count_target = count;
	coreid_t *cores = NULL;
	core_t *source = topology_get_core_from_idx(prg->source_idx);
	for(i = 1; i <= TOPOLOGY_MAXIMUM_DISTANCE; i++) {
		coreid_t cores_count = topology_get_cores_at_distance(source->idx, i, &cores);
		for(j = 0; j < cores_count; j++) {
			if(count == 0) {
				goto increase_buffer_exit;
			}
			core_t *c = &topology_cores[cores[j]];
			pthread_mutex_lock(&(c->lock));
			if(c->sourced == false && sched_is_core_allotted(prg, cores[j]) == false) {
				if(c->assignments > 0 && assigned_idx < count) {
					hc[assigned_idx] = i;
					assigned[assigned_idx++] = c;
				} else {
					program_worker_set(prg, c->idx, i);
					sched_core_allot(c->idx);
					count--;
				}
			}
			pthread_mutex_unlock(&(c->lock));
		}
	}
	// at this point not enough idle cores where found
	// time-share
	for(i = 0; i < assigned_idx; i++) {
		if(count == 0) {
			goto increase_buffer_exit;
		}
		core_t *c = assigned[i];
		pthread_mutex_lock(&(c->lock));
		if(c->assignments < SCHED_CORE_ASSIGNED_THRESHOLD) {
			program_worker_set(prg, c->idx, hc[i]);
			sched_core_allot(c->idx);
			count--;
		}
		pthread_mutex_unlock(&(c->lock));
	}
increase_buffer_exit:
	cprint(_SYS_VERBOSE_SCHED, "SCHED: Increased allotment by %d\n", count_target - count);
	if(cores != NULL)
		free(cores);
	return count_target - count;
}

/**
 * Populate a buffer of cores to release from an allotment based on distance
 * @param buffer	The pre-allocated buffer to populate
 * @param source	The source of the allotment to decrement
 * @param count		The number of cores to find
 * @return			The number of cores found
 */
coreid_t sched_allotment_decrease(program_t prg, coreid_t count) {
	if(count == 0)
		return 0;
	// buffer to hold distance-viable cores that are already assigned
	coreid_t num = 0;
	// iterate the source's distances vectors to find viable cores
	coreid_t i,j;
	coreid_t *cores = NULL;
	core_t *source = topology_get_core_from_idx(prg->source_idx);
	for(i = TOPOLOGY_MAXIMUM_DISTANCE; i > 0; i--) {
		coreid_t cores_idx = topology_get_cores_at_distance(source->idx, i, &cores);
		for(j = 0; j < cores_idx; j++) {
			if(num == count) {
				goto decrease_buffer_exit;
			}
			core_t *c = &topology_cores[cores[j]];
			pthread_mutex_lock(&(c->lock));
			if(sched_is_core_allotted(prg, cores[j]) == true) {
				program_worker_let(prg, c->idx);
				sched_core_release(c->idx);
				num++;
			}
			pthread_mutex_unlock(&(c->lock));
		}
	}
decrease_buffer_exit:
	if(cores != NULL)
		free(cores);
	return num;
}

/**
 * Create a new allotment, including the cores in a buffer.
 * If source points to NULL, a source is found using distance based algorithm
 * If buffer is NULL use distance algorithm to populate it
 * @param prg		Pointer placeholder for a program descriptor. If not NULL, settings forced
 * @param source_id	The physical id of the core to use as source. If settings forced, should mirror the value in prg
 * @param count		The number of cores to allot, if settings forced should mirror the value in prg
 * @return			The number of new cores allotted, including the source
 */
coreid_t sched_allotment_create(program_t *prg, int source_id, coreid_t count, rt_vtbl_t vtbl) {
	core_t *source = NULL;
	if(source_id < 0)
		source = sched_core_source_get();
	else
		source = topology_get_core_from_id(source_id);
	assert(source != NULL);
	if(*prg == NULL) {
		// get program
		*prg = program_init(source->idx, vtbl);
		if(*prg == NULL)
			return 0;
	}
	sched_core_source(source);
	/** @TODO: release source from programs it's assigned to */
	count = sched_allotment_increase(*prg, count);
	source->assignments++;
	return count + 1;
}

/**
 * Release an allotment completely.
 * @param source	Pointer to the source core for the program whose allotment is released
 */
void sched_allotment_release(program_t prg) {
	// first release each core
	int i;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++) {
		if(prg->workers[i] == NULL)
			continue;
		core_t *c = topology_get_core_from_idx(prg->workers[i]->widx);
		pthread_mutex_lock(&(c->lock));
		// release a worker
		program_worker_let(prg, c->idx);
		/** @NOTE: Not using sched_core_release()
		 *  since setting source's assigned bitmap is redundant */
		// reduce the assignments for this worker
		c->assignments--;
		pthread_mutex_unlock(&(c->lock));
	}
	core_t *source = topology_get_core_from_idx(prg->source_idx);
	sched_core_unsource(source);
}

/**
 * Mark a core as source
 * @param source	The core being marked as source
 */
void sched_core_source(core_t *source){
	pthread_mutex_lock(&(source->lock));
	source->sourced = 1;
	pthread_mutex_unlock(&(source->lock));
}

/**
 * Unmark a core as source
 * @param source	The core being unmarked as source
 */
void sched_core_unsource(core_t *source){
	pthread_mutex_lock(&(source->lock));
	source->sourced = 0;
	// allocate and initialize assigned bitmap
	pthread_mutex_unlock(&(source->lock));
}

/**
 * Mark a core as assigned to a program
 * @param cid		The id of the core being marked
 * @param source	The source of the program owning the core
 */
static inline void sched_core_allot(coreid_t idx){
	topology_cores[idx].assignments++;
}

/**
 * Unmark a core as assigned to a program
 * @param cid		The id of the core being unmarked
 * @param source	The source of the program releasing the core
 */
static inline void sched_core_release(coreid_t idx){
	topology_cores[idx].assignments--;
}

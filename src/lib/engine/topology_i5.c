/* 
 * File:   topology_i5.c
 * Author: Georgios Varisteas
 *
 * Created on May 7, 2014, 8:49 PM
 */


#ifndef __i5__
#define __i5__
#endif

#include <stdlib.h>
#include "topology.h"

core_t topology_cores[TOPOLOGY_CORE_COUNT];			///< all available cores
coreid_t topology_maximum_distance = TOPOLOGY_MAXIMUM_DISTANCE;


void topology_cores_init(void) {
	coreid_t i;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++){
		core_t *c = &topology_cores[i];
		c->assignments = 0;
		c->id = i + TOPOLOGY_CORE_FIRST;
		c->idx = i;
		c->sourced = 0;
		pthread_mutex_init(&(c->lock), NULL);
	}
}

coreid_t topology_get_distance_of_cores(coreid_t a, coreid_t b) {
	return 1;
}

coreid_t topology_get_cores_at_distance(coreid_t core, coreid_t dist, coreid_t **buffer) {
	coreid_t buffer_idx = 0;
	if(dist == 1) {
		*buffer = calloc(TOPOLOGY_CORE_COUNT - 1, sizeof(coreid_t));
		coreid_t v;
		for(v = TOPOLOGY_CORE_FIRST; v <= TOPOLOGY_CORE_LAST; v++) {
			if(v - TOPOLOGY_CORE_FIRST != core)
				(*buffer)[buffer_idx++] = v - TOPOLOGY_CORE_FIRST;
		}
	}
	return buffer_idx;
}

/**
 * Get the core_t object given the core's external id (provided and recognizable by the OS)
 * @NOTE This function makes no check on the existence of the core.
 * @param id	the id code of the desired core
 * @return the respective core object
 */
core_t *topology_get_core_from_id(coreid_t id) {
	return &topology_cores[id - TOPOLOGY_CORE_FIRST];
}

/**
 * Get the core_t object given the core's internal index (not recognizable by the OS)
 * @NOTE This function makes no check on the existence of the core.
 * @param idx	the index of the desired core
 * @return the respective core object
 */
core_t *topology_get_core_from_idx(coreid_t idx) {
	return &topology_cores[idx];
}


/*
 * File:   topology_xeon_phi.c
 * Author: Georgios Varisteas
 *
 * Created on February 6, 2014, 1:56 PM
 */


#ifndef __XEON_PHI__
#define __XEON_PHI__
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
	return abs((topology_cores[a].id / TOPOLOGY_CORES_PER_NODE_COUNT) - (topology_cores[b].id / TOPOLOGY_CORES_PER_NODE_COUNT));
}

coreid_t topology_get_cores_at_distance(coreid_t core, coreid_t dist, coreid_t **buffer) {
	coreid_t node2, node = topology_cores[core].id / TOPOLOGY_CORES_PER_NODE_COUNT;
	*buffer = calloc(2*TOPOLOGY_CORES_PER_NODE_COUNT, sizeof(coreid_t));
	coreid_t i, buffer_idx = 0, v;
	// dist to the left or same node if dist is 0
	node2 = node - dist;
	if(node2 < 0)
		node2 += TOPOLOGY_NODE_COUNT;
	v = node2 * TOPOLOGY_CORES_PER_NODE_COUNT;
	for(i = 0; i < TOPOLOGY_CORES_PER_NODE_COUNT; i++) {
		if(v >= TOPOLOGY_CORE_FIRST && v <= TOPOLOGY_CORE_LAST && v != core) {
			(*buffer)[buffer_idx++] = v - TOPOLOGY_CORE_FIRST;
		}
		v++;
	}
	if(dist == 0)
		goto topology_get_cores_at_distance_exit;
	// dist to the right
	node2 = node + dist;
	if(node2 >= TOPOLOGY_NODE_COUNT)
		node2 -= TOPOLOGY_NODE_COUNT;
	v = node2 * TOPOLOGY_CORES_PER_NODE_COUNT;
	for(i = 0; i < TOPOLOGY_CORES_PER_NODE_COUNT; i++) {
		if(v >= TOPOLOGY_CORE_FIRST && v <= TOPOLOGY_CORE_LAST && v != core) {
			(*buffer)[buffer_idx++] = v - TOPOLOGY_CORE_FIRST;
		}
		v++;
	}
topology_get_cores_at_distance_exit:
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


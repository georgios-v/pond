/*
 * File:   topology_gothmog.c
 * Author: Georgios Varisteas
 *
 * Created on January 27, 2014, 9:49 AM
 */

#ifndef __GOTHMOG__
#define __GOTHMOG__
#endif

#include <stdlib.h>
#include "topology.h"

core_t topology_cores[TOPOLOGY_CORE_COUNT];			///< all available cores
coreid_t topology_maximum_distance = TOPOLOGY_MAXIMUM_DISTANCE;
coreid_t topology_node_dist[TOPOLOGY_NODE_COUNT][TOPOLOGY_NODE_COUNT] = 
							{
								{1, 2, 2, 3, 2, 3, 2, 3},
								{2, 1, 3, 2, 3, 2, 3, 2},
								{2, 3, 1, 2, 2, 3, 2, 3},
								{3, 2, 2, 1, 3, 2, 3, 2},
								{2, 3, 2, 3, 1, 2, 2, 3},
								{3, 2, 3, 2, 2, 1, 3, 2},
								{2, 3, 2, 3, 2, 3, 1, 2},
								{3, 2, 3, 2, 3, 2, 2, 1},
							};


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
	coreid_t node_a = topology_cores[a].id / TOPOLOGY_CORES_PER_NODE_COUNT;
	coreid_t node_b = topology_cores[b].id / TOPOLOGY_CORES_PER_NODE_COUNT;
	return topology_node_dist[node_a][node_b];
}

coreid_t topology_get_cores_at_distance(coreid_t core, coreid_t dist, coreid_t **buffer) {
	coreid_t node = topology_cores[core].id / TOPOLOGY_CORES_PER_NODE_COUNT;
	int i,j;
	if(dist == 1)
		i = 1;
	else if(dist == 2)
		i = 4;
	else if(dist == 3)
		i = 3;
	else {
		*buffer = NULL;
		return 0;
	}
	coreid_t buffer_idx = 0;
	*buffer = calloc(TOPOLOGY_NODE_COUNT*TOPOLOGY_CORES_PER_NODE_COUNT, sizeof(coreid_t));
	for(i = 0; i < TOPOLOGY_NODE_COUNT; i++) {
		// check node-to-node distance
		if(topology_node_dist[node][i] == dist) {
			// iterate nodes within node i
			for(j = 0; j < TOPOLOGY_CORES_PER_NODE_COUNT; j++) {
				coreid_t cid = i*TOPOLOGY_CORES_PER_NODE_COUNT + j;
				if(cid >= TOPOLOGY_CORE_FIRST && cid <= TOPOLOGY_CORE_LAST && cid - TOPOLOGY_CORE_FIRST != core) {
					(*buffer)[buffer_idx++] = cid - TOPOLOGY_CORE_FIRST;
				}
			}
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

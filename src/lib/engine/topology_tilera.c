/*
 * File:   topology_gothmog.c
 * Author: Georgios Varisteas
 *
 * Created on January 27, 2014, 9:49 AM
 */

#ifndef __TILERA__
#define __TILERA__
#endif

#include <stdlib.h>
#include "topology.h"

#define TOPOLOGY_CORES_PER_ROW 8
#define TOPOLOGY_CORES_PER_COLUMN 7

#define TOPOLOGY_CORE_SPECIAL_ID 58
#define TOPOLOGY_CORE_SPECIAL_IDX (TOPOLOGY_CORE_COUNT - 1)

core_t topology_cores[TOPOLOGY_CORE_COUNT];			///< all available cores
coreid_t topology_maximum_distance = TOPOLOGY_MAXIMUM_DISTANCE;

typedef struct topology_coords {
	coreid_t x:3;
	coreid_t y:3;
} topology_coords_t;

topology_coords_t topology_coords[TOPOLOGY_CORE_COUNT];
//bitvector_t topology_distances[TOPOLOGY_CORE_COUNT];

void topology_cores_init(void) {
	coreid_t i, idx = TOPOLOGY_CORE_FIRST;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++){
		core_t *c = &topology_cores[i];
		c->assignments = 0;
		c->id = idx++;
		c->idx = i;
		c->sourced = 0;
		pthread_mutex_init(&(c->lock), NULL);
/*		
		bitvector_init(&topology_distances[i], TOPOLOGY_CORE_COUNT - from, TOPOLOGY_MAXIMUM_DISTANCE);
		coreid_t edx = 0, j, from = 1, v[topology_distances[i].count], k = 0;
		for(j = from; j < TOPOLOGY_CORE_COUNT; j++) {
			v[k] = edx + k + 1;
			if(k == topology_distances[i].count) {
				bitvector_set_entry(&topology_distances[i], v, edx);
				k = 0;
				edx++;
			}
		}
		from++;
*/
		coreid_t x = 0, y = 0;
		topology_coords[i].x = x++;
		topology_coords[i].y = y;
		if(x == TOPOLOGY_CORES_PER_ROW) {
			x = 0;
			y++;
		}

	}
	topology_cores[TOPOLOGY_CORE_SPECIAL_IDX].id = TOPOLOGY_CORE_SPECIAL_ID;
}

coreid_t topology_get_distance_of_cores(coreid_t a, coreid_t b) {
	coreid_t dist = 0; // initialized to cover the case of (a == b)
//	if(a < b)
//		dist = bitvector_get(&topology_distances[a], b);
//	else if(b > a)
//		dist = bitvector_get(&topology_distances[b], a);
	if(a != b)
		dist = abs(topology_coords[a].x - topology_coords[b].x) + abs(topology_coords[a].y - topology_coords[b].y);
	return dist;
}

coreid_t topology_get_cores_at_distance(coreid_t core, coreid_t dist, coreid_t **buffer) {
	coreid_t buffer_idx = 0;
	if(dist == 0)
		goto topology_get_cores_at_distance_exit;
	coreid_t i, v;
	coreid_t max = 4*(dist - 1) + 4;
	*buffer = calloc(max, sizeof(coreid_t));
	int x_plus_dist = topology_coords[core].x + dist;
	int x_minus_dist = topology_coords[core].x - dist;
	int y_plus_dist = topology_coords[core].y + dist;
	int y_minus_dist = topology_coords[core].y - dist;
	int v_x = x_plus_dist;
	int v_y = topology_coords[core].y;
	int step_x = -1;
	int step_y = 1;
	for(i = 0; i < max; i++) {
		if((v_x > 0 && v_x <= TOPOLOGY_CORES_PER_ROW) && 
			(v_y > 0 && v_y <= TOPOLOGY_CORES_PER_COLUMN)) {
			v = (v_y - 1)*TOPOLOGY_CORES_PER_ROW + v_x - 1;
			if(v >= TOPOLOGY_CORE_FIRST && v <= TOPOLOGY_CORE_LAST && v - TOPOLOGY_CORE_FIRST != core) {
				(*buffer)[buffer_idx++] = v - TOPOLOGY_CORE_FIRST;
			}
			if(v == TOPOLOGY_CORE_SPECIAL_ID && v != core) {
				(*buffer)[buffer_idx++] = TOPOLOGY_CORE_SPECIAL_IDX;
			}
		}
		v_y += step_y;
		v_x += step_x;
		if(v_x == x_minus_dist)
			step_x = -step_x;
		if(v_y == y_plus_dist || v_y == y_minus_dist)
			step_y = -step_y;
		if(v_x == x_plus_dist && v_y == topology_coords[core].y)
			break;
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
	if(id == TOPOLOGY_CORE_SPECIAL_ID)
		return &topology_cores[TOPOLOGY_CORE_SPECIAL_IDX];
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



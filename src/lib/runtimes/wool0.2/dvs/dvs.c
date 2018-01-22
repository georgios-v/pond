/*
 * File:   dvs.c
 * Author: georgios
 *
 * Created on January 20, 2014, 9:15 AM
 */

#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <util.h>
#include "types.h"
#include "dvs.h"

//typedef char bool;

#define hc(A,B) (abs(dvs_cores[A].x - dvs_cores[B].x) + abs(dvs_cores[A].y - dvs_cores[B].y))
#define BOUND_CORE(_count, _worker) (_worker >= 0 && _worker < _count)

/**
 * Find and store the source's V (victims set)
 * NOTE: V_sz (size of list) is zeroed!
 * @param allot	holds the set of workers and total count
 */
static coreid_t dvs_init_thief_S(opt_allotment_t allot) {
	opt_worker_dvs_t w = (opt_worker_dvs_t) (allot->workers[0]);
	w->V_sz = 0;
	coreid_t sz = (allot->count > 5? 5: allot->count), v;
	for(v = 1; v < sz; v++) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "%d:%d\n", v, allot->workers[v]->idx);
	}
	return sz;
}

/**
 * Find and store a worker's O (outer victims) in V (victims set)
 * NOTE: V_sz (size of list) is not zeroed
 * @param idx	the index of this worker in the workers set
 * @param allot	holds the set of workers and total count
 * @param i		the index of this worker in it's own zone
 * @param d		the hc of the worker from the source
 * @param in_X	boolean denoting if this worker is in X (true) or F (false)
 */
static coreid_t dvs_init_thief_VO(coreid_t idx, opt_allotment_t allot, coreid_t i, coreid_t d, bool in_X) {
	coreid_t v, diff = 4*d + i, sz = 0;
	opt_worker_dvs_t w = (opt_worker_dvs_t) (allot->workers[idx]);
	v = idx + diff;
	if(BOUND_CORE(allot->count,v) && v != idx) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "O1:%d:%d\n", v, allot->workers[v]->idx);
		sz++;
	}
	v = v + 1;
	if(BOUND_CORE(allot->count,v) && v != idx) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "O2:%d:%d\n", v, allot->workers[v]->idx);
		sz++;
	}
	// if this is in X there are three possible outer victims
	if(in_X) {
		// Special indexing for north peak worker, has to modulo to the end of the zone
		v = v + ((i == 0?1:0)*diff) - 1;
		if(BOUND_CORE(allot->count,v) && v != idx) {
			w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
			cprint(_USR_DEBUG_DVS, "O3:%d:%d\n", v, allot->workers[v]->idx);
			sz++;
		}
	}
	return sz;
}

/**
 * Find and store a worker's I (inner victims) in V (victims set)
 * NOTE: V_sz (size of list) is not zeroed
 * @param idx	the index of this worker in the workers set
 * @param allot	holds the set of workers and total count
 * @param i		the index of this worker in it's own zone
 * @param d		the hc of the worker from the source
 * @param in_F_or_Z	boolean denoting if this worker is in (Z or F) (true) or X (false)
 */
static coreid_t dvs_init_thief_VI(coreid_t idx, opt_allotment_t allot, coreid_t i, coreid_t d, bool in_F_or_Z) {
	coreid_t v, sz = 0;
	opt_worker_dvs_t w = (opt_worker_dvs_t) (allot->workers[idx]);
	v = idx - (4 * (d - 1)) - (d==1?i:0);
//	cprint(_USR_DEBUG_DVS, "I1 ** %d %d %d %d\n", idx, d, i, v);
	if(BOUND_CORE(allot->count,v) && v != idx) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "I1:%d:%d\n", v, allot->workers[v]->idx);
		sz++;
	}
	if(in_F_or_Z) {
		v = v - 1;
		if(BOUND_CORE(allot->count,v) && v != idx) {
			w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
			cprint(_USR_DEBUG_DVS, "I2:%d:%d\n", v, allot->workers[v]->idx);
			sz++;
		}
	}
	return sz;
}

/**
 * Find and store a worker's in X-core special V (victims set) extension
 * NOTE: V_sz (size of list) is not zeroed
 * @param idx	the index of this worker in the workers set
 * @param allot	holds the set of workers and total count
 * @param d		the hc of the worker from the source
 */
static coreid_t dvs_init_thief_X(coreid_t idx, opt_allotment_t allot, coreid_t d) {
	if(d != 1)
		return 0;
	opt_worker_dvs_t w = (opt_worker_dvs_t) (allot->workers[idx]);
	coreid_t sz = 0, v = idx;
	while(1) {
		if(++v > 4)
			v = 1;
		if(v == idx)
			break;
		if(BOUND_CORE(allot->count,v) && v != idx) {
			w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
			cprint(_USR_DEBUG_DVS, "X:%d:%d\n", v, allot->workers[v]->idx);
			sz++;
		}
	}
	return sz;
}

/**
 * Find and store a worker's in Z (or P) special V (victims set) extension
 * NOTE: V_sz (size of list) is not zeroed
 * @param idx	the index of this worker in the workers set
 * @param allot	holds the set of workers and total count
 * @param d		the hc of the worker from the source
 * @param is_PP	boolean denoting if this worker is the single northern peak of Z (true) or any other member of Z (false)
 */
static coreid_t dvs_init_thief_Z(coreid_t idx, opt_allotment_t allot, coreid_t d, bool is_PP) {
	opt_worker_dvs_t w = (opt_worker_dvs_t) (allot->workers[idx]);
	coreid_t sz = 0, v;
	v = idx + 1;
	if(BOUND_CORE(allot->count,v) && v != idx) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "Z1:%d:%d\n", v, allot->workers[v]->idx);
		sz++;
	}
	v = idx + ((is_PP == 0?0:1)*4*d) - 1;
	if(BOUND_CORE(allot->count,v) && v != idx) {
		w->V[w->V_sz++] = (opt_worker_dvs_t) (allot->workers[v]);
		cprint(_USR_DEBUG_DVS, "Z2:%d:%d\n", v, allot->workers[v]->idx);
		sz++;
	}
	return sz;
}



void dvs_init_island(opt_allotment_t allot) {
	if(allot->count == 0)
		return;
	// find max d
	coreid_t idx = 0, i, j, k, lx = 0, lf = 0, d = dvs_find_d(allot->count);
	cprint(_USR_DEBUG_DVS, "Init island (d:%d count:%d)\n", d, allot->count);
	cprint(_USR_DEBUG_DVS, "Source: %d (\n", idx);
	dvs_init_thief_S(allot);
	cprint(_USR_DEBUG_DVS, ")\n");
	for(i = 1; i < d; i++) {
		lx = 1;
		lf = 0;
		for(j = 0; j < 4; j++) {
			if(++idx == allot->count) {
				goto dvs_init_island_exit;
			}
			// init X
			cprint(_USR_DEBUG_DVS, "Worker X: %d lx:%d d:%d (\n", idx, lx, i);
			((opt_worker_dvs_t) allot->workers[idx])->V_sz = 0;
			dvs_init_thief_VI(idx, allot, lx, i, 0);
			dvs_init_thief_X(idx, allot, i);
			dvs_init_thief_VO(idx, allot, lx, i, 1);
			cprint(_USR_DEBUG_DVS, ")\n");
			lx++;
			for(k = 0; k < i - 1; k++) {
				if(++idx == allot->count) {
					goto dvs_init_island_exit;
				}
				// init F
				cprint(_USR_DEBUG_DVS, "Worker F: %d lx:%d d:%d (\n", idx, lf, i);
				((opt_worker_dvs_t) allot->workers[idx])->V_sz = 0;
				dvs_init_thief_VO(idx, allot, lf, i, 0);
				dvs_init_thief_VI(idx, allot, lf, i, 1);
				cprint(_USR_DEBUG_DVS, ")\n");
			}
		}
	}
	lx = 1;
	lf = 0;
	for(j = 0; j < 4; j++) {
		if(++idx == allot->count) {
			goto dvs_init_island_exit;
		}
		if(d == 1) {
			// init X
			cprint(_USR_DEBUG_DVS, "Worker X: %d lx:%d d:%d (\n", idx, lx, i);
			((opt_worker_dvs_t) allot->workers[idx])->V_sz = 0;
			dvs_init_thief_VI(idx, allot, lx++, i, 0);
			dvs_init_thief_X(idx, allot, i);
			cprint(_USR_DEBUG_DVS, ")\n");
		} else {
			// init P
			cprint(_USR_DEBUG_DVS, "Worker P: %d lx:%d d:%d (\n", idx, lx, i);
			((opt_worker_dvs_t) allot->workers[idx])->V_sz = 0;
			dvs_init_thief_Z(idx, allot, d, (lx == 1));
			dvs_init_thief_VI(idx, allot, lx++, d, 0);
			cprint(_USR_DEBUG_DVS, ")\n");
		}
		for(k = 0; k < d - 1; k++) {
			if(++idx == allot->count) {
				goto dvs_init_island_exit;
			}
			// init Z
			cprint(_USR_DEBUG_DVS, "Worker Z: %d lf:%d d:%d (\n", idx, lf, i);
			((opt_worker_dvs_t) allot->workers[idx])->V_sz = 0;
			dvs_init_thief_Z(idx, allot, d, 0);
			dvs_init_thief_VI(idx, allot, lf++, d, 1);
			cprint(_USR_DEBUG_DVS, ")\n");
		}

	}
	dvs_init_island_exit:
		return;
}

coreid_t dvs_find_d(coreid_t count) {
	coreid_t i = 0, j = 1;
	while(1) {
		if(j >= count) {
			return i;
		}
		i++;
		j += 4*i;
	}
	return 0;
}

/* 
 * File:   program.c
 * Author: Georgios Varisteas
 *
 * Created on March 11, 2014, 11:25 PM
 */

#include "topology.h"
#include <config.h>
#include <pond_types.h>
#include "types.h"
#include "util.h"
#include <bench.h>
#include "bitmap.h"
#include "program.h"

extern pond_thread_t *_threads;

program_t _programs[MAX_PROGRAMS];

worker_t worker_init(program_t prg, coreid_t idx, coreid_t hc) {
	worker_t w = checked_malloc(sizeof(*w));
	w->active = false;
	w->widx = idx;
#if(BENCHMARKING == 1)
	w->bench = stat_bench_init();
#endif
	w->hc = hc;
	w->program = prg;
	w->opt = NULL;
	if(prg->vtbl->init_w) {
		w->opt = prg->vtbl->init_w(prg->opt, idx);
	}
	return w;
}

void worker_release(worker_t w) {
#if(BENCHMARKING == 1)
	free(w->bench);
#endif
	free(w);
}

program_t program_init(coreid_t source_idx, rt_vtbl_t vtbl) {
	int i = 0;
	while(_programs[i] != NULL) {
		if(++i == MAX_PROGRAMS)
			return NULL;
	}
	program_t prg = (program_t) checked_malloc(sizeof(*prg));
	_programs[i] = prg;
	prg->active = false;
	prg->pid = i;
	prg->vtbl = vtbl;
	prg->source_idx = source_idx;
	prg->workers = (worker_t *) checked_malloc(sizeof(worker_t) * (TOPOLOGY_CORE_COUNT));
	prg->count = 0;
	prg->allotment = (opt_allotment_t) checked_malloc(sizeof(*prg->allotment));
	prg->allotment->workers = (opt_worker_t *) checked_malloc(sizeof(opt_worker_t) * (TOPOLOGY_CORE_COUNT));
	prg->allotment->idx = (coreid_t *) checked_malloc(sizeof(coreid_t) * (TOPOLOGY_CORE_COUNT));
	prg->allotment->count = 0;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++) {
		prg->workers[i] = NULL;
		prg->allotment->workers[i] = NULL;
		prg->allotment->idx[i] = TOPOLOGY_CORE_COUNT; // impossible value as placeholder
	}
	if(prg->vtbl->init_p)
		prg->opt = prg->vtbl->init_p(TOPOLOGY_CORE_COUNT);
	program_worker_set(prg, source_idx, 0);
	prg->helper = NULL;
	return prg;
}

void program_fini(program_t prg) {
	prg->active = false;
	for(int i = 0; i < TOPOLOGY_CORE_COUNT; i++) {
		if(prg->workers[i] != NULL)
			while(prg->workers[i]->active != false && _threads[i]->pause == 0);
	}
}

void program_release(program_t prg) {
	// release the slot early
	_programs[prg->pid] = NULL;
	// start shutting down
	int i;
	for(i = 0; i < TOPOLOGY_CORE_COUNT; i++){
		if(prg->workers[i] != NULL) {
			if(prg->vtbl->kil_w)
				prg->vtbl->kil_w(prg->workers[i]->opt);
			prg->workers[i]->opt = NULL;
			worker_release(prg->workers[i]);
			prg->workers[i] = NULL;
		}
	}
	if(prg->vtbl->kil)
		prg->vtbl->kil(prg->opt);
	prg->opt = NULL;
	free(prg->workers);
	prg->workers = NULL;
	free(prg->allotment->workers);
	prg->allotment->workers = NULL;
	free(prg->allotment->idx);
	prg->allotment->idx = NULL;
	free(prg->allotment);
	prg->allotment = NULL;
	free(prg);
}

worker_t program_worker_set(program_t prg, coreid_t idx, coreid_t hc) {
	cprint(_SYS_VERBOSE_INC, "Adding worker %d\n", idx);
	// check if worker has been added before or not
	if(prg->workers[idx] == NULL) {
		prg->workers[idx] = worker_init(prg, idx, hc);
		prg->count++;
	}
	// add to allotment
	prg->allotment->workers[prg->allotment->count] = prg->workers[idx]->opt;
	prg->allotment->idx[idx] = prg->allotment->count++;
	// activate
	prg->workers[idx]->active = true;
	// make selectable by thread
	_threads[idx]->programs[prg->pid] = prg->workers[idx];
	return prg->workers[idx];
}

worker_t program_worker_let(program_t prg, coreid_t idx) {
	// Remove program from thread
	_threads[idx]->programs[prg->pid] = NULL;
	// deactivate worker
	prg->workers[idx]->active = false;
	// remove worker from allotment
	prg->allotment->workers[prg->allotment->idx[idx]] = NULL;
	int i;
	for(i = prg->allotment->idx[idx]; i < prg->allotment->count - 1; i++) {
		prg->allotment->workers[i] = prg->allotment->workers[i + 1];
		prg->allotment->idx[prg->allotment->workers[i]->idx] = i;
	}
	prg->allotment->workers[--prg->allotment->count] = NULL;
	prg->allotment->idx[idx] = TOPOLOGY_CORE_COUNT;
	return prg->workers[idx];
}

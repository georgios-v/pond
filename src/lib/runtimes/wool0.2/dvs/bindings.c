#ifndef BARRELFISH
#include <malloc.h>
#endif
#include <util.h>
#include <pond_types.h>
#include "types.h"
#include <wool0.h>
#include "dvs.h"

extern volatile bool __pond_stop_work;

void __wool0_dvs_sync(volatile Task *t, balarm_t a);
bool __wool0_dvs_work(opt_worker_t s, opt_allotment_t allot);

opt_program_t __wool0_dvs_init_program(coreid_t cores);
opt_worker_t __wool0_dvs_init_worker(opt_program_t opt, coreid_t idx);
void __wool0_dvs_init(opt_program_t opt, opt_allotment_t allot);
void __wool0_dvs_fini(opt_program_t opt, opt_allotment_t allot);
bool __wool0_dvs_kil_worker(opt_worker_t opt);

struct __runtime_vtbl vtbl_wool0_dvs = {
	.init_p = __wool0_dvs_init_program,
	.init_w = __wool0_dvs_init_worker,
	.init = __wool0_dvs_init,
	.work = __wool0_dvs_work,
	.inc = NULL,
	.dec = NULL,
	.fini = __wool0_dvs_fini,
	.kil_w = __wool0_dvs_kil_worker,
	.kil = NULL,
	.hlp = NULL,
	.bnc = __wool0_get_bench_string,
};

/**
 *
 * @param cores Total available cores
 * @param workers Number of currently allotted workers
 * @return
 */
opt_program_t __wool0_dvs_init_program(coreid_t cores) {
	return (opt_program_t) (uint64_t) wool0_get_nstealable(cores, INIT_N_STEALABLE);
}

opt_worker_t __wool0_dvs_init_worker(opt_program_t opt, coreid_t idx) {
	int i;
	opt_worker_dvs_t w = checked_malloc(sizeof(*w));
	w->idx = idx;
	w->sync = __wool0_dvs_sync;
	w->dq_size = INIT_WORKER_DQ_SIZE;
	w->dq_base = (Task *) memalign(LINE_SIZE, w->dq_size * sizeof (Task));
	w->dq_bot = w->dq_base;
	w->dq_lock = &(w->the_lock);
	pthread_mutex_init(w->dq_lock, NULL);
	for (i = 0; i < w->dq_size; i++) {
		w->dq_base[i].f = T_BUSY;
		w->dq_base[i].balarm = NOT_STOLEN;
		w->dq_base[i].stealable = ((i < ((opt_program_wool0_dvs_t) (uint64_t) opt)) ? 1 : 0);
		w->dq_base[i].self = w;
	}
	w->dq_top = w->dq_base;
	w->Vid = 0;
	w->V = (opt_worker_dvs_t *) checked_malloc(sizeof (opt_worker_dvs_t) * MAX_V_SZ);
	w->V_sz = 0;
#if (BENCHMARKING == 1)
	w->bnc = checked_malloc(sizeof(*(w->bnc)));
	w->bnc->non_useful_time = 0;
	w->bnc->steals_f = 0;
	w->bnc->steals_s = 0;
	w->bnc->tasks = 0;
#endif
	
	return (opt_worker_t) w;
}


/**
 *
 * @param opt	The program runtime-specific opt structure
 * @param count Number of currently allotted workers
 * @param allot	The allotment structure
 */
void __wool0_dvs_init(opt_program_t opt, opt_allotment_t allot) {
	dvs_init_island(allot);
}

void __wool0_dvs_fini(opt_program_t opt, opt_allotment_t allot) {
	
}

bool __wool0_dvs_kil_worker(opt_worker_t opt){
	opt_worker_dvs_t w = (opt_worker_dvs_t) opt;
	free(w->dq_base);
	free(w->V);
#if (BENCHMARKING == 1)
	free(w->bnc);
#endif
	free(w);
	return true;
}


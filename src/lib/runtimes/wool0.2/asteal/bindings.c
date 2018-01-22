#ifdef BARRELFISH
#include <barrelfish/barrelfish.h>
#include <barrelfish/types.h>
#include <barrelfish/threads.h>
#include <barrelfish/waitset.h>
#include <barrelfish/deferred.h>
#else
#include <malloc.h>
#endif
#include <util.h>
#include <pond_types.h>
#include "types.h"
#include <wool0.h>
#include <bench.h>

bool __wool0_work(opt_worker_t s, opt_allotment_t allot);
extern volatile bool __pond_stop_work;

void __wool0_sync(volatile Task *t, balarm_t a);

opt_program_t __wool0_asteal_init_program(coreid_t cores);
opt_worker_t __wool0_asteal_init_worker(opt_program_t opt, coreid_t idx);
bool __wool0_kil_worker(opt_worker_t opt);
void __wool0_asteal_init(opt_program_t opt, opt_allotment_t allot);
void __wool0_asteal_fini(opt_program_t opt, opt_allotment_t allot);
void __wool0_asteal_helper(opt_program_t opt, opt_allotment_t allot, dmc_t dmc);

struct __runtime_vtbl vtbl_wool0_asteal = {
	.init_p = __wool0_asteal_init_program,
	.init_w = __wool0_asteal_init_worker,
	.init = __wool0_asteal_init,
	.work = __wool0_work,
	.inc = NULL,
	.dec = NULL,
	.fini = __wool0_asteal_fini,
	.kil_w = __wool0_kil_worker,
	.kil = NULL,
	.hlp = __wool0_asteal_helper,
	.bnc = __wool0_get_bench_string,
};

opt_program_t __wool0_asteal_init_program(coreid_t cores) {
	opt_program_wool0_asteal_t opt = checked_malloc(sizeof(*opt));
	opt->stealable = wool0_get_nstealable(cores, INIT_N_STEALABLE);
	opt->non_stealable = checked_malloc(sizeof(cycles_t)*cores);
	opt->L = 0;
	int i;
	for(i = 0; i < cores; i++)
		opt->non_stealable[i] = 0;
	return (opt_program_t) opt;
}

opt_worker_t __wool0_asteal_init_worker(opt_program_t opt, coreid_t idx) {
	int i;
	opt_worker_wool0_t w = checked_malloc(sizeof(*w));
	w->idx = idx;
	w->sync = __wool0_sync;
	w->dq_size = INIT_WORKER_DQ_SIZE;
	w->dq_base = (Task *) memalign(LINE_SIZE, w->dq_size * sizeof (Task));
	w->dq_bot = w->dq_base;
	w->dq_lock = &(w->the_lock);
	pthread_mutex_init(w->dq_lock, NULL);
	for (i = 0; i < w->dq_size; i++) {
		w->dq_base[i].f = T_BUSY;
		w->dq_base[i].balarm = NOT_STOLEN;
		w->dq_base[i].stealable = ((i < ((opt_program_wool0_asteal_t) opt)->stealable) ? 1 : 0);
		w->dq_base[i].self = w;
	}
	w->dq_top = w->dq_base;
	w->i = 0;
	w->victim_idx = idx;
#if (BENCHMARKING == 1)
	w->bnc = checked_malloc(sizeof(*(w->bnc)));
	w->bnc->non_useful_time = 0;
	w->bnc->steals_f = 0;
	w->bnc->steals_s = 0;
	w->bnc->tasks = 0;
#endif
	return (opt_worker_t) w;
}

void __wool0_asteal_init(opt_program_t opt, opt_allotment_t allot) {
	opt_program_wool0_asteal_t prg = (opt_program_wool0_asteal_t) opt;
	prg->d = allot->count;
}

void __wool0_asteal_fini(opt_program_t opt, opt_allotment_t allot) {
	free(((opt_program_wool0_asteal_t) opt)->non_stealable);
	free((opt_program_wool0_asteal_t) opt);
}

void __wool0_asteal_helper(opt_program_t opt, opt_allotment_t allot, dmc_t dmc) {
	opt_program_wool0_asteal_t prg = (opt_program_wool0_asteal_t) opt;
	cycles_t L;
	if(prg->L) {
		L = tsc_diff(prg->L);
	} else {
		prg->L = tsc_get;
		return;
	}
	int a = allot->count;
	// previous allotment desire
	int d = prg->d;
	// get n
	cycles_t n = 0; //non-steal usage == mug+steal
	int i;
	for(i = 0; i < a; i++) {
		opt_worker_wool0_t w = ((opt_worker_wool0_t)allot->workers[i]);
		cycles_t nn = w->bnc->non_useful_time;
		n += nn - prg->non_stealable[w->idx];
		prg->non_stealable[w->idx] = nn;
	}
	cprint(_SYS_DEBUG_HELPER == 1, "Helper n: %ld %ld %d %d %ld\n", n, L, ASTEAL_DELTA, allot->count, (L * ASTEAL_DELTA * allot->count) / 10);
	// new desire for this quanta
	int nd = d; // default to balanced
	// run the decision algorithm
	// div 10 is so that delta can be an integer
	// this n is the opposite from the paper
	// there non-steal is the sum of work+mug cycles
	// here non-steal means wasted cycles
	if(n >= (L * ASTEAL_DELTA * a) / 10) {
		nd = d/ASTEAL_RESP;	// inefficient, decrease
	} else if (a == d) {
		nd = ASTEAL_RESP*d;	// efficient and satisfied, increase
	}
	
	prg->d = nd;
	// enforce
	if (nd < a) {
		cprint(_SYS_VERBOSE_HELPER == 1, "Helper DEC\n");
		dmc->type = DMC_DEC;
		dmc->count = a - nd;
	} else if (nd > a) {
		cprint(_SYS_VERBOSE_HELPER == 1, "Helper INC\n");
		dmc->type = DMC_INC;
		dmc->count = nd - a;
	}
	prg->L = tsc_get;
	return;
}

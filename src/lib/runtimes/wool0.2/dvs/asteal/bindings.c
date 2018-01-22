#ifdef BARRELFISH
#include <barrelfish/barrelfish.h>
#include <barrelfish/types.h>
#include <barrelfish/threads.h>
#include <barrelfish/waitset.h>
#include <barrelfish/deferred.h>
#endif
#include <pond_types.h>
#include "types.h"
#include <wool0.h>

extern volatile bool __pond_stop_work;

bool __wool0_dvs_work(opt_worker_t s, opt_allotment_t allot);
void __wool0_dvs_sync(volatile Task *t, balarm_t a);

opt_program_t __wool0_dvs_init_program(coreid_t cores);
opt_worker_t __wool0_dvs_init_worker(opt_program_t opt, coreid_t idx);
void __wool0_dvs_init(opt_program_t opt, opt_allotment_t allot);
void __wool0_dvs_fini(opt_program_t opt, opt_allotment_t allot);
bool __wool0_dvs_kil_worker(opt_worker_t opt);
void __wool0_asteal_helper(opt_program_t opt, opt_allotment_t allot, dmc_t dmc);

struct __runtime_vtbl vtbl_wool0_dsteal = {
	.init_p = __wool0_dvs_init_program,
	.init_w = __wool0_dvs_init_worker,
	.init = __wool0_dvs_init,
	.work = __wool0_dvs_work,
	.inc = NULL,
	.dec = NULL,
	.fini = __wool0_dvs_fini,
	.kil_w = __wool0_dvs_kil_worker,
	.kil = NULL,
	.hlp = __wool0_asteal_helper,
	.bnc = __wool0_get_bench_string,
};

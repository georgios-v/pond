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
#include "../dvs.h"


#define PALIRRIA_INC_THRESHOLD 3
#define PALIRRIA_DEC_THRESHOLD 0
#define PALIRRIA_SAME_ZONE_DMC_THRESHOLD 50

#define palirria_get_bag_size(_idx) \
	((opt_worker_dvs_t) (allot->workers[_idx]))->dq_top - ((opt_worker_dvs_t) (allot->workers[_idx]))->dq_bot + 1

extern volatile bool __pond_stop_work;

void __wool0_dvs_sync(volatile Task *t, balarm_t a);
bool __wool0_dvs_work(opt_worker_t s, opt_allotment_t allot);

opt_program_t __wool0_dvs_init_program(coreid_t cores);
opt_worker_t __wool0_dvs_init_worker(opt_program_t opt, coreid_t idx);
void __wool0_dvs_init(opt_program_t opt, opt_allotment_t allot);
void __wool0_dvs_fini(opt_program_t opt, opt_allotment_t allot);
bool __wool0_dvs_kil_worker(opt_worker_t opt);
void __wool0_palirria_helper(opt_program_t opt, opt_allotment_t allot, dmc_t dmc);

struct __runtime_vtbl vtbl_wool0_palirria = {
	.init_p = __wool0_dvs_init_program,
	.init_w = __wool0_dvs_init_worker,
	.init = __wool0_dvs_init,
	.work = __wool0_dvs_work,
	.inc = __wool0_dvs_init,
	.dec = __wool0_dvs_init,
	.fini = __wool0_dvs_fini,
	.kil_w = __wool0_dvs_kil_worker,
	.kil = NULL,
	.hlp = __wool0_palirria_helper,
	.bnc = __wool0_get_bench_string,
};

static inline float palirria_missing_d_perc(coreid_t d, coreid_t c) {
	return 100 - ((25*c)/d);
}

static inline coreid_t palirria_get_last_zone_count(coreid_t d, coreid_t count) {
	int i, total = 1;
	for(i = 1; i < d; i++) {
		total += 4*d;
	}
	// at this point total has the number of cores up to d - 1
	return count - total;
}

void __wool0_palirria_helper(opt_program_t opt, opt_allotment_t allot, dmc_t dmc){
	bool inc = true;
	bool dec = false;
	uint32_t sz;

	coreid_t idx = 0, i, j, k, lx = 0, lf = 0, d = dvs_find_d(allot->count);
	for(i = 1; i < d; i++) {
		lx = 0;
		lf = 0;
		for(j = 0; j < 4; j++) {
			if(++idx == allot->count) {
				goto palirria_helper_continue;
			}
			/**
			 * idx is X
			 */
//			w = (opt_worker_dvs_t) (allot->workers[idx]);
//			sz = w->dq_top - w->dq_bot + 1;
			sz = palirria_get_bag_size(idx);
			// check against threshold
			// if a single X has less than L do not INC
			if(sz <= PALIRRIA_INC_THRESHOLD)
				inc = false;

			lx++;
			// skip F workers
			idx += i - 1;
			if(idx >= allot->count) {
				goto palirria_helper_continue;
			}
		}
	}
	lx = 0;
	lf = 0;
	for(j = 0; j < 4; j++) {
		if(++idx == allot->count) {
			goto palirria_helper_continue;
		}
		/**
		 * idx is P
		 */
		sz = palirria_get_bag_size(idx);
		if(sz > PALIRRIA_DEC_THRESHOLD) {
			dec = false;
			break;
		}

		for(k = 0; k < d - 1; k++) {
			if(++idx == allot->count) {
				goto palirria_helper_continue;
			}
			/**
			 * idx is Z
			 */
			sz = palirria_get_bag_size(idx);
			if(sz > PALIRRIA_DEC_THRESHOLD) {
				dec = false;
				break;
			}
		}
		if(dec == false)
			break;
	}

	palirria_helper_continue:

	// run the decision algorithm
	// if either dec or inc but not both, change accordingly
	// enforce
	if(dec ^ inc) {
		coreid_t c = palirria_get_last_zone_count(d, allot->count);
		coreid_t m = palirria_missing_d_perc(d, c);
		if (dec) {
			cprint(_SYS_VERBOSE_HELPER == 1, "Helper DEC\n");
			dmc->type = DMC_DEC;
			// if less than THRESHOLD is missing, release last zone
			if(m < PALIRRIA_SAME_ZONE_DMC_THRESHOLD || d == 1) {
				dmc->count = c;
			} else { // if more than THRESHOLD is missing, release last AND previous to last zones
				dmc->count = c + 4*(d - 1);
			}
		} else {
			cprint(_SYS_VERBOSE_HELPER == 1, "Helper INC\n");
			dmc->type = DMC_INC;
			// if last zone more than THRESHOLD full, complete and add new zone
			if(m > PALIRRIA_SAME_ZONE_DMC_THRESHOLD || d == 0) {
				dmc->count = c + 4*(d + 1);
			} else { // if last zone less than THRESHOLD full, complete it
				dmc->count = c;
			}
		}
	}
	return;
}


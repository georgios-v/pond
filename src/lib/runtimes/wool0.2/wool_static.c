#include "types.h"
#include <wool0.h>

unsigned int wool0_get_nstealable(coreid_t count, unsigned int start_value) {
	int i;
	for (i = count; i > 0; i >>= 1) {
		start_value++;
	}
	if (count == 1) {
		start_value = 0;
	}
	return start_value;
}

void wool0_update_dq_top(Task *q) {
	q->self->dq_top = q;
}

void wool0_task_inc_lcl(Task *q) {
#if (BENCHMARKING == 1)
	q->self->bnc->tasks++;
#endif
}

void __wool0_SYNC(volatile Task *t, balarm_t a) {
	wool_get_self(t)->sync(t,a);
}

Task *__wool0_get_top(opt_worker_t self) {
	Task *low = ((Worker *)self)->dq_base, *high = low + ((Worker *)self)->dq_size;
	wool_lock(((Worker *)self)->dq_lock);
	while (low < high - 1) {
		Task *mid = low + (high - low) / 2;

		if (mid->f == T_BUSY && mid->balarm == NOT_STOLEN) {
			high = mid;
		} else {
			low = mid;
		}
	}
	wool_unlock(((Worker *)self)->dq_lock);

	return low;
}

balarm_t __wool0_sync_get_balarm(Task *t) {
	Worker *self;
	balarm_t a;

	self = wool_get_self(t);
	wool_lock(self->dq_lock);
	a = t->balarm;
	wool_unlock(self->dq_lock);
	return a;
}

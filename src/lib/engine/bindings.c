#ifdef BARRRELFISH
#include <config.h>
#include <util.h>
#include "types.h"
#include "bindings.h"
#include <pond_types.h>

#include <pthread_compat.h>



extern pond_thread_t *_threads;
program_t get_program_to_init(void) {
	return get_self(_threads)->init->program;
}

program_t get_program_to_fini(coreid_t sid) {
	return get_self(_threads)->programs[sid];
}
#endif


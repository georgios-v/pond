/*
 * File:   pond.c
 * Author: Georgios Varisteas
 *
 * Created on April 30, 2012, 11:04 AM
 */
#ifdef BARRELFISH
#include <barrelfish/types.h>
#include <barrelfish/barrelfish.h>
#include <barrelfish_kpi/init.h>
#include <barrelfish/dispatch.h>
#include <barrelfish/terminal.h>
#ifndef __ARM__
#include <acpi_client/acpi_client.h>
#include <skb/skb.h>
#else
#include <barrelfish/barrelfish.h>
#include <barrelfish/nameservice_client.h>
#include <barrelfish/spawn_client.h>
#include <if/monitor_defs.h>
#endif
#else
#include <sys/sysinfo.h>
#endif
#include "topology.h"
#include <config.h>
#include <pond_types.h>
#include <bench.h>
#include "types.h"
#include "shell.h"
#include "syssched.h"
#include "master.h"
#include "program.h"
#include <util.h>
#include "helper.h"

extern volatile coreid_t __pond_master_core_id;

extern pond_thread_t *_threads;
extern volatile bool cmd_wait;

mutex_t print_mutex;					///< Generic mutex lock for non-interleaved printing

uint8_t pid_cur = 0;

int execute_program_custom(struct __pond_app *app, char runtime, int source_id, coreid_t core_count, 
		uint64_t helper_sleep, int argc, char **argv, int argv_offset) {
	if(_SYS_DEBUG_MAIN) {
		debug("EXEC %s", argv[argv_offset]);
		for (uint8_t i = 1; i < argc; i++) {
			debug(" %s", argv[argv_offset + i]);
		}
		debug("\n");
	}
	/*********************
	 * Get chosen runtime
	 *********************/
	rt_vtbl_t vtbl = NULL;
	if(runtime == _SYS_EMPTY_RUNTIME)
		vtbl = get_runtime_default(app);
	else
		vtbl = check_runtime(app, runtime);
	if(!vtbl) {
		error("Unsupported runtime %c requested. Aborting\n", runtime);
		return 1;
	}
	/*******************
	 * Create allotment
     *******************/
	program_t prg = NULL;
	core_count = sched_allotment_create(&prg, source_id, core_count - 1, vtbl);
	if(core_count == 0 || prg == NULL) {
		error("Abort: Program slots full. Try again later.\n");
		return 1;
	}
	cprint(_SYS_VERBOSE_MAIN, "EXEC %s on %d (%d)\n", argv[0], prg->source_idx, core_count);
	// setup main function
	program_init_t _program_init = get_worker(_threads, prg->source_idx)->init;
	_program_init->src = app->cmd;
	_program_init->argc = argc;
	_program_init->argv = argv;
	_program_init->argv_offset = argv_offset;
	_program_init->wait = cmd_wait;
	// store the program
	_program_init->program = prg;
	// activate program
	_program_init->active = true;
	// @XXX: Fix this. maybe force last minute no-pause in worker if program_init in place
	// force wake up source worker
	if(vtbl->hlp != NULL) {
		if(helper_sleep == 0){
			error("Adaptive runtime selected. Forcing helper thread creation\n");
			helper_sleep = HELPER_SLEEP_TIME;
		}
		prg->helper = helper_create(TOPOLOGY_CORE_HELPER, prg, helper_sleep);
		while(prg->helper->state != TRD_PAUSED);
	}
	worker_resume_get(_threads, prg->source_idx);
	return 0;
}

int main(int argc, char** argv) {
	coreid_t __pond_nr_available_cores = 1;
#ifdef BARRELFISH
	// get my core id
	__pond_master_core_id = disp_get_core_id();
	cprint(_SYS_VERBOSE_MAIN == 1, "master core id: %c\n", __pond_master_core_id);
// get the number of available cores
#ifndef __ARM__
	// connect to the SKB
	errval_t err = skb_client_connect();
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "connection to SKB failed");
	}
	// ask SKB for number of cores
	err = skb_execute("available_nr_cores(Nr),write(nrcores(Nr)).");
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "Could not determine the number of cores."
				"Stay with one core...\n");
	}
	// read SKB reply
	err = skb_read_output("nrcores(%u)", (unsigned int *) &__pond_nr_available_cores);
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "Could not determine the number of cores <2>."
				"Stay with one core...\n");
	}
#else
	// get a binding to the monitor of the current dispatcher
	struct monitor_binding *mb = get_monitor_binding();

	// Get the number of cores
	mb->rx_vtbl.num_cores_reply = num_cores_reply;
	request_done = false;
	errval_t err = mb->tx_vtbl.num_cores_request(mb, NOP_CONT);
	if (err_is_fail(err)) {
		USER_PANIC_ERR(err, "sending num_cores_request failed");
	}
	// Wait for the reply
	while (!request_done) {
		messages_wait_and_handle_next();
	}
#endif
#else
	__pond_master_core_id = TOPOLOGY_CORE_MASTER;
	// set affinity to core 0
	setaffinity(pthread_self(), __pond_master_core_id);
	__pond_nr_available_cores = get_nprocs();
#endif
	assert(__pond_nr_available_cores >= TOPOLOGY_CORE_COUNT);
	debug("nr available cores: %d\n", __pond_nr_available_cores);
	// initiate benchmarking
#if (BENCHMARKING == 1)
	bench_init();
#endif
	pthread_mutex_init(&print_mutex, NULL);
	// run the scheduler
	runscheduler();
	// run the environment
	shell_run_env(argc, argv);
	return (EXIT_SUCCESS);
}



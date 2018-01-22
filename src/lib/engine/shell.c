/*
 * File:   shell.c
 * Author: georgios
 *
 * Created on January 17, 2014, 8:04 PM
 */

#include <string.h>
#include "config.h"
#include "types.h"
#include "util.h"
#include "shell.h"
#include "pond.h"
#include "../toc.h"

extern volatile bool __pond_stop_work;

static int shell_help(int argc, char *argv[]);
static int shell_reset(int argc, char *argv[]);
static int shell_poweroff(int argc, char *argv[]);
static int shell_get_mem_info(int argc, char *argv[]);
static int shell_on_core(int argc, char *argv[]);
static int shell_with_runtime(int argc, char *argv[]);
static int shell_custom(int argc, char *argv[]);
static int shell_run_custom(char runtime, int source_id, uint8_t core_count, uint64_t helper_sleep, 
		int argc, char *argv[], int argv_offset);

static struct __pond_cmd *shell_find_command(const char *name);
static struct __pond_app *shell_find_app(const char *name);

static int shell_makeargs(char *cmdline, char *argv[]);
static size_t shell_getaline(char *input, size_t size);

#ifdef BARRELFISH
static bool acpi_connected = false;
#endif

volatile bool cmd_wait = true;
pthread_mutex_t cmd_wait_mutex;
pthread_cond_t cmd_wait_cond;


rt_vtbl_t get_runtime(char vtbl){
	for (int i = 0; i < ENTRIES(apps); i++) {
		if(rts[i].code == vtbl)
			return rts[i].vtbl;
	}
	return NULL;
}

rt_vtbl_t get_runtime_default(struct __pond_app *app){
	return get_runtime(app->vtbl[0]);
}

rt_vtbl_t check_runtime(struct __pond_app *app, char vtbl){
	char c;
	int i = 0;
	do{
		c = app->vtbl[i++];
		if(vtbl == c)
			return get_runtime(c);
	} while(c != '\0');
	return NULL;
}

void shell_exec_commands(char **tmpinput, uint8_t cmd_count) {
	int i, exitcode = 0;
	for (i = 0; i < cmd_count; i++) {
		if (strlen(tmpinput[i]) < 1)
			continue;
		struct __pond_cmd *cmd;
		int cmd_argc; // = 0;
		char **cmd_argv = (char **) checked_malloc(sizeof (char *) *64); // Support a max of 64 cmd args
		cprint(_SYS_DEBUG_SHELL, " **** ARGV: %p\n", cmd_argv);
		cmd_argc = shell_makeargs(tmpinput[i], cmd_argv);
		/* check for trailing '&' (== run in background) */
		cmd_wait = true;
		if (cmd_argc > 0) {
			size_t len = strlen(cmd_argv[cmd_argc - 1]);
			if (len > 0)
				if (cmd_argv[cmd_argc - 1][len - 1] == '&') {
					cmd_wait = false;
					// remove '&' character from args
					if (len == 1) {
						cmd_argc--;
					} else {
						cmd_argv[cmd_argc - 1][len - 1] = '\0';
					}
				}
		}
		if (cmd_argc == 0) {
			continue;
		} else if ((cmd = shell_find_command(cmd_argv[0])) != NULL) {
			exitcode = cmd->cmd(cmd_argc, cmd_argv);
			cmd_wait = false;
		} else {
			exitcode = shell_run_custom(_SYS_EMPTY_RUNTIME, _SYS_EMPTY_SOURCE, _SYS_EMPTY_CORE_COUNT, _SYS_EMPTY_HELPER_TIME, cmd_argc, cmd_argv, 0);
		}
		debug("SHELL: exitcode: %d, wait: %d\n", exitcode, cmd_wait);
		// wait if it succeeds
		if (exitcode == 0 && cmd_wait) {
			//	exitcode = wait_domain_id(domain_id);
			verbose("SHELL: sleeping\n");
			pthread_mutex_lock(&cmd_wait_mutex);
			pthread_cond_wait(&cmd_wait_cond, &cmd_wait_mutex);
			pthread_mutex_unlock(&cmd_wait_mutex);
		}
	}
}

void shell_parse_commands(char *input, size_t len){
	if (!len)
		return;
	char *tmpinput[9]; // Support up to 9 consecutive commands
	char buffer[255];
	uint8_t cmd_count = 0, j = 0;

	// split input commands on semicolon
	bool delim = false;
	for (int i = 0; i < len; i++) {
		if (input[i] == ';') {
			delim = true;
			buffer[j] = '\0';
			tmpinput[cmd_count] = strdup(buffer);
			debug("SHELL: Found command %d: %s\n", cmd_count, tmpinput[cmd_count]);
			cmd_count++;
			if (cmd_count > 9) {
				verbose("WARNING: Issued commands after 9th ignored!\n");
				break;
			}
			j = 0;
		} else {
			delim = false;
			buffer[j++] = input[i];
		}
	}
	if (!delim) {
		buffer[j] = '\0';
		tmpinput[cmd_count] = strdup(buffer);
		cmd_count++;
		debug("SHELL: Found command %d: %s\n", cmd_count, tmpinput[cmd_count - 1]);
	}
	verbose("SHELL: Found %d commands\n", cmd_count);
	shell_exec_commands(tmpinput, cmd_count);
	for (int i = 0; i < cmd_count; i++) {
		free(tmpinput[i]);
	}
}

void shell_parse_script(int argc, char *argv[]) {
	char *input;
	size_t len = 0;
	size_t read;
	FILE *fp;

	print("Running script!\n");

	fp = fopen(argv[1], "r");
	if(fp == NULL)
		exit(EXIT_FAILURE);

	// read one line at a time
	// there can be up to 9 commands per line
	// separated by semicolon
	while ((read = getline(&input, &len, fp)) != -1) {
		// exclude EOL
		if(input[read - 1] == '\n')
			input[read - 1] = '\0';
		// print command
		print("> %s\n", input);
		shell_parse_commands(input, len);
		len = 0;
	}	
}

int shell_run_shell(int argc, char *argv[]) {
	char input[MAX_LINE];

#ifdef BARRELFISH
	unsigned stdin_sources = 0;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "serial") == 0) {
			stdin_sources |= TERMINAL_SOURCE_SERIAL;
		} else if (strcmp(argv[i], "keyboard") == 0) {
			stdin_sources |= TERMINAL_SOURCE_KEYBOARD;
		}
	}
	// fallback default: use serial, as before
	if (stdin_sources == 0)
		stdin_sources = TERMINAL_SOURCE_SERIAL;
	errval_t e = terminal_want_stdin(stdin_sources);
	assert(err_is_ok(e));
#endif

	print("Running interactive shell!\n");

	while(__pond_stop_work == false) {
		print("> ");
		fflush(stdout);
		size_t len = shell_getaline(input, MAX_LINE); // Wait for command string
		shell_parse_commands(input, len);
	}
	return 0;
}


void shell_run_env(int argc, char *argv[]) {
	// initiate command lock
	pthread_mutex_init(&cmd_wait_mutex, NULL);
	pthread_cond_init(&cmd_wait_cond, NULL);
	// start
	print("%s v1.2 -- pleased to meet you!\n", argv[0]);
	if(argc > 1)
		// run the script
		shell_parse_script(argc, argv);
	else
		// run the shell
		shell_run_shell(argc, argv);
}

static struct __pond_cmd commands[] = {
	{"help", shell_help, "Output usage information about given shell command"},
	{"reset", shell_reset, "Reset machine"},
	{"poweroff", shell_poweroff, "Power down machine"},
	{"q", shell_poweroff, "Power down machine"},
	{"mem", shell_get_mem_info, "Display amount of free memory in the system"},
	{"on", shell_on_core, "Start program on specified core"},
	{"oc", shell_on_core, "Start program on specified core"},
	{"oncore", shell_on_core, "Start program on specified core"},
	{"c", shell_custom, "Start program with specific arguments"},
	{"custom", shell_custom, "Start program with specific arguments"},
	{"rt", shell_with_runtime, "Start program with custom runtime"},
	{"runtime", shell_with_runtime, "Start program with custom runtime"},
};

static int shell_with_runtime(int argc, char *argv[]){
	if (argc < 3) {
		print("Usage: %s <V> <program> [args]\n\tV\tThe runtime to use. Select among (a d p v w)\n", argv[0]);
		return 1;
	}
	char runtime = argv[1][0];
	argc -= 2;
//	argv += 2;
	return shell_run_custom(runtime, _SYS_EMPTY_SOURCE, _SYS_EMPTY_CORE_COUNT, _SYS_EMPTY_HELPER_TIME, argc, argv, 2);
}

static int shell_on_core(int argc, char *argv[]) {
	if (argc < 3) {
		print("Usage: %s <core id> <program> [args]\n", argv[0]);
		return 1;
	}
	int source = atoi(argv[1]);
	argc -= 2;
//	argv += 2;
	return shell_run_custom(_SYS_EMPTY_RUNTIME, source, _SYS_EMPTY_CORE_COUNT, _SYS_EMPTY_HELPER_TIME, argc, argv, 2);
}

static int shell_custom(int argc, char *argv[]) {
	if (argc < 6) {
		print("Usage: %s <V> <S> <D> <H> <program> [args]\n\tV\tThe runtime to use. Select among (p b w)\n\tS\tThe Source core to start on, set to 0 for default\n\tD\tThe initial Diaspora value\n\tH\tThe helper sleep period in us (set to 0 to deactivate)\n", argv[0]);
		return 1;
	}
	char V = argv[1][0];
	coreid_t S = atoi(argv[2]);
	uint8_t C = atoi(argv[3]);
	uint64_t H = atol(argv[4]);
	argc -= 5;
//	argv += 5;
	return shell_run_custom(V, S, C, H, argc, argv, 5);
}

static int shell_run_custom(char runtime, int source_id, uint8_t core_count, uint64_t helper_sleep, int argc, char *argv[], int argv_offset) {
	// Get app
	struct __pond_app *app = shell_find_app(argv[argv_offset]);
	if (app == NULL) {
		error("App '%s' not found\n", argv[argv_offset]);
		return 1;
	}
	return execute_program_custom(app, runtime, source_id, core_count, helper_sleep, argc, argv, argv_offset);
}

#ifdef BARRELFISH
static int shell_reset(int argc, char *argv[]) {
	stopscheduler();
#ifndef __ARM__
	if (!acpi_connected) {
		int r = connect_to_acpi();
		assert(r == 0);
		acpi_connected = true;
	}
	return acpi_reset();
#else
	return 0;
#endif
}

static int shell_poweroff(int argc, char *argv[]) {
	stopscheduler();
#ifndef __ARM__
	if (!acpi_connected) {
		int r = connect_to_acpi();
		assert(r == 0);
		acpi_connected = true;
	}
	return acpi_sleep(4);
#else
	return 0;
#endif
}

static int shell_get_mem_info(int argc, char *argv[]) {
	struct mem_rpc_client *mc = get_mem_client();
	assert(mc != NULL);
	errval_t err;
	genpaddr_t available, total;

	err = ram_available(&available, &total);
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "available");
		return EXIT_FAILURE;
	}
	print("Free memory: %" PRIuGENPADDR " bytes\n", available);
	print("Total memory: %" PRIuGENPADDR " bytes\n", total);
	return EXIT_SUCCESS;
}
#else
static int shell_reset(int argc, char *argv[]) {
	exit(1);
	return 0;
}

static int shell_poweroff(int argc, char *argv[]) {
	__pond_stop_work = true;
	return 0;
}

static int shell_get_mem_info(int argc, char *argv[]) {
	return 0;
}
#endif

static int shell_help(int argc, char *argv[]) {
	struct __pond_cmd *cmd;
	struct __pond_app *app;
	if (argc == 1) {
		print("available commands:\n");
		for (int i = 0; i < ENTRIES(commands); i++) {
			print("%-15s", commands[i].name);
			if (((i + 1) % 5) == 0) {
				print("\n");
			}
		}
		print("\n");
		for (int i = 0; i < ENTRIES(apps); i++) {
			print("%-15s", apps[i].name);
			if (((i + 1) % 5) == 0) {
				print("\n");
			}
		}
		print("\n");
		return 0;
	}
	if ((cmd = shell_find_command(argv[1])) != NULL) {
		print("%s: %s\n", argv[1], cmd->usage);
		return 0;
	} else if ((app = shell_find_app(argv[1])) != NULL) {
		print("%s: %s\n", argv[1], app->usage);
		return 0;
	} else {
		print("%s: %s: command not found\n", argv[0], argv[1]);
		return 1;
	}
}

static size_t shell_getaline(char *input, size_t size) {
	int i = 0, in;
	do {
		in = getchar();
		if (in == '\b' || in == 0x7f /* DEL */) {
			if (i > 0) {
				i--;
				putchar('\b');
				//puts("\033[1X");
			}
		} else if (in != '\n' && i < size - 1) {
			input[i++] = in;
			putchar(in);
		}
		fflush(stdout);
	} while (in != '\n');
	assert(i < size);
	input[i] = '\0';
	putchar('\n');
	fflush(stdout);
	return i;
}

static struct __pond_cmd *shell_find_command(const char *name) {
	for (int i = 0; i < ENTRIES(commands); i++) {
		struct __pond_cmd *cmd = &commands[i];
		if (strcmp(name, cmd->name) == 0) {
			return cmd;
		}
	}
	return NULL;
}

static struct __pond_app *shell_find_app(const char *name) {
	for (int i = 0; i < ENTRIES(apps); i++) {
		struct __pond_app *app = &apps[i];
		if (strcmp(name, app->name) == 0) {
			return app;
		}
	}
	return NULL;
}

static int shell_makeargs(char *cmdline, char *argv[]) {
	char *p = cmdline;
	bool inquote = false;
	int argc = 0;
	if (p == NULL) return 0;
	while (*p == ' ') p++;
	if (*p == '\0') return 0;
	for (argv[argc++] = p; *p != '\0'; p++) {
		if (*p == '"') {
			inquote = !inquote;
			*p = ' '; // mega-kludge!
		} else if (*p == ' ' && !inquote) {
			*p++ = '\0';
			// Skip any redundant whitespace
			while (*p == ' ') {
				p++;
			}
			if (*p != '\0') {
				argv[argc++] = p;
				if (*p == '"') {
					inquote = true;
					*p = ' '; // mega-kludge
				}
			}
		}
	}
	return argc;
}

void shell_cmd_finished(void) {
#if (_SYS_VERBOSE_SHELL == 1)
	print("SHELL: sleeping\n");
#endif
	pthread_mutex_lock(&cmd_wait_mutex);
	pthread_cond_signal(&cmd_wait_cond);
	pthread_mutex_unlock(&cmd_wait_mutex);
}

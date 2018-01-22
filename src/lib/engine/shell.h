#ifndef SHELL_H
#define SHELL_H

#define _SYS_EMPTY_SOURCE -1
#define _SYS_EMPTY_RUNTIME '\0'
#define _SYS_EMPTY_CORE_COUNT 5
#define _SYS_EMPTY_HELPER_TIME 0

rt_vtbl_t get_runtime(char vtbl);
rt_vtbl_t get_runtime_default(struct __pond_app *app);
rt_vtbl_t check_runtime(struct __pond_app *app, char vtbl);
void shell_run_env(int argc, char *argv[]);
void shell_cmd_finished(void);

#endif /* SHELL_H */

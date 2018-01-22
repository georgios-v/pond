/* 
 * File:   wool_compat.h
 * Author: Georgios Varisteas
 *
 * Created on May 31, 2011, 1:51 PM
 */

#ifndef __PTHREAD_COMPAT_H__
#define __PTHREAD_COMPAT_H__

#ifdef BARRELFISH
	typedef struct thread *thread_t;
	typedef struct thread_mutex mutex_t;
	typedef struct thread_cond cond_t;
#include <barrelfish/barrelfish.h>
#include <barrelfish/threads.h>

#define __SIZEOF_PTHREAD_ATTR_T 36

enum {
  PTHREAD_SCOPE_SYSTEM,
#define PTHREAD_SCOPE_SYSTEM    PTHREAD_SCOPE_SYSTEM
  PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS   PTHREAD_SCOPE_PROCESS
};

#ifndef __cplusplus
#define PTHREAD_MUTEX_INITIALIZER \
    { .locked = false, .queue = NULL, .lock = 0 }
#else
#define PTHREAD_MUTEX_INITIALIZER                                \
    { false, (struct thread *)NULL, 0, (struct thread *)NULL }
#endif

#ifndef __cplusplus
#define PTHREAD_COND_INITIALIZER \
    { .queue = NULL, .lock = 0 }
#else
#define PTHREAD_COND_INITIALIZER \
    { (struct thread *)NULL, 0 }
#endif

typedef struct thread_mutex pthread_mutex_t;
typedef struct thread_cond pthread_cond_t;

// pthread_t is originally defined as unsigned long int
// in order to hold just the header of the struct pthread
// created at pthread_create:
// typedef unsigned long int pthread_t;
// this is ok because pthread_join requires nothing more
// than the header.
// However, barrelfish threading returns the full structure
// upon thread_create, and requires it back in thread_join
// thus pthread_t has to be defined as struct thread

typedef struct thread *pthread_t;

typedef union {
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
} pthread_attr_t;

//int rand_r(unsigned int * seedptr);

#define pthread_create(_ph1, _ph2, __start_routine, _data) thread_create(__start_routine, _data)
#define pthread_join(_thread, _retval) thread_join(_thread, _retval)
#define pthread_exit(_ret) thread_exit()

static inline __attribute__((always_inline)) int pthread_create_on(coreid_t core_id, pthread_t ph1, void *ph2, thread_func_t __start_routine, void *data) {
//	ph1 = backend_thread_create_on(core_id, __start_routine, data);
	return 0;
}

#define pthread_attr_init(__attr) 0
#define pthread_attr_setscope(__attr, __scope) 0

/* ph exists for compatibility with pthreads, should be NULL */
#define pthread_mutex_init(_mutex, _ph) thread_mutex_init(_mutex)
#define pthread_mutex_lock(_mutex) thread_mutex_lock(_mutex)
#define pthread_mutex_unlock(_mutex) thread_mutex_unlock(_mutex)
// In POSIX success is 0, in Barrelfish it is reversed
#define pthread_mutex_trylock(_mutex) (!thread_mutex_trylock(_mutex))

#define pthread_cond_init(_cond, __cond_attr) thread_cond_init(_cond)
#define pthread_cond_signal(_cond) thread_cond_signal(_cond)
#define pthread_cond_broadcast(_cond) thread_cond_broadcast(_cond)
#define pthread_cond_wait(_cond, _mutex) thread_cond_wait(_cond, _mutex)

typedef unsigned int pthread_key_t;

#define pthread_getspecific(_key) thread_get_tls()
#define pthread_setspecific(_key, _arg) thread_set_tls(_arg)
#else
#include <pthread.h>
	typedef pthread_t thread_t;
	typedef pthread_mutex_t mutex_t;
	typedef pthread_cond_t cond_t;
#endif // BARRELFISH

#endif // __PTHREAD_COMPAT_H__


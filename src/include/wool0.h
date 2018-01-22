/*
   This file is part of Wool, a library for fine-grained independent
   task parallelism

   Copyright (C) 2009- Karl-Filip Faxen
	  kff@sics.se

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.
 */

#ifndef WOOL0_H
#define WOOL0_H

#define WOOL_VERBOSE 1
#define WOOL_DEBUG 1

#include "pthread_compat.h"

#ifndef FINEST_GRAIN
#define FINEST_GRAIN 2000
#endif

#ifndef WOOL_DEFER_BOT_DEC
#define WOOL_DEFER_BOT_DEC 1
#endif

#ifndef WOOL_DEFER_NOT_STOLEN
#define WOOL_DEFER_NOT_STOLEN 1
#endif

#ifndef THE_SYNC
#define THE_SYNC 0
#endif

#ifndef LINE_SIZE
#define LINE_SIZE 64  /* A common value for current processors */
#endif

#define SMALL_BODY             2
#define MEDIUM_BODY          100
#define LARGE_BODY  FINEST_GRAIN

#define P_SZ (sizeof(void *))
#define I_SZ (sizeof(int))
#define L_SZ (sizeof(long int))

#define PAD(x,b) ( ( (b) - ((x)%(b)) ) & ((b)-1) ) /* b must be power of 2 */
#define ROUND(x,b) ( (x) + PAD( (x), (b) ) )

#ifndef TASK_PAYLOAD
#define TASK_PAYLOAD 20*8
#endif

typedef volatile unsigned long exarg_t;

#define SFENCE        __asm__ volatile( "sfence" )
#define MFENCE        __asm__ volatile( "mfence" )
/* { volatile int i=1; EXCHANGE( i, i ); } */
#define PREFETCH(a)   /*  */
#define EXCHANGE(R,M) __asm__ volatile ( "xchg   %1, %0" : "+m" (M), "+r" (R) )
#define CAS(R,M,V)  __asm__ volatile ( "lock cmpxchg %2, %1" \

#define COMPILER_FENCE  __asm__ volatile( "" )
// x86, amd64 and SPARC v9 can do without a store barrier
#define STORE_PTR_REL(addr,val) (*(addr) = (val))
#define STORE_INT_REL(addr,val) (*(addr) = (val))

typedef long unsigned int balarm_t;
typedef long long unsigned int hrtime_t;

typedef pthread_mutex_t wool_lock_t;
typedef pthread_cond_t wool_cond_t;

#define wool_lock(l)      pthread_mutex_lock( l )
#define wool_unlock(l)    pthread_mutex_unlock( l )
#define wool_trylock(l)   pthread_mutex_trylock( l )
#define wool_wait(c,l)    pthread_cond_wait( c, l )
#define wool_signal(c)    pthread_cond_signal( c )
#define wool_broadcast(c) pthread_cond_broadcast( c )

#ifndef WOOL0_TYPES_H
	typedef struct opt_worker_wool0 Worker;
#endif

#define TASK_COMMON_FIELDS(ty)    \
  void (*f)(struct _Task *, ty);  \
  balarm_t balarm;                \
  unsigned stealable;             \
  Worker *self;

struct _Task;

typedef struct {
	TASK_COMMON_FIELDS(struct _Task *)
} __wool_task_common;

#define COMMON_FIELD_SIZE sizeof( __wool_task_common )

typedef struct _Task {
	TASK_COMMON_FIELDS(struct _Task *)
	char p1[ PAD(COMMON_FIELD_SIZE, P_SZ) ];
	char d[ TASK_PAYLOAD ];
	char p2[ PAD(ROUND(COMMON_FIELD_SIZE, P_SZ) + TASK_PAYLOAD, LINE_SIZE) ];
} Task;

#define WRAPPER_TYPE void (*)( struct _Task *, struct _Task * )

#define T_BUSY ((WRAPPER_TYPE) 0)
#define T_DONE ((WRAPPER_TYPE) 1)
#define T_LAST ((WRAPPER_TYPE) 1)

#define NOT_STOLEN  ( (balarm_t) 0 )
#define STOLEN_BUSY ( (balarm_t) 1 ) // Not used with LF
#define STOLEN_DONE ( (balarm_t) 2 )
#define B_LAST      STOLEN_DONE

#define wool_get_self( t ) ( t->self )
#ifdef BARRELFISH
#define memalign(ALIGN, SIZE)  malloc(SIZE)
#endif

int CALL_main(Task *, int, char **);


#ifndef INIT_WORKER_DQ_SIZE
#define INIT_WORKER_DQ_SIZE 100000
#endif
#ifndef INIT_N_STEALABLE
#define INIT_N_STEALABLE 20
#endif

#define SYNC( f )       (__dq_top--, SYNC_##f( __dq_top ) )
#define SPAWN( f, ... ) ( SPAWN_##f( __dq_top ,##__VA_ARGS__ ), __dq_top++ )
#define CALL( f, ... )  ( CALL_##f( __dq_top , ##__VA_ARGS__ ) )
#define FOR( f, ... )   ( CALL( TREE_##f , ##__VA_ARGS__ ) )
#define ROOT_CALL( f, ...) ( CALL_##f( __wool0_get_top(self) , ##__VA_ARGS__ ) )
#define ROOT_FOR( f, ... ) ( ROOT_CALL( TREE_##f , ##__VA_ARGS__ ) )

#include <wool0_static.h>

#endif	// WOOL0_H
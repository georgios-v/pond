#ifndef WOOL0_RUNTIME_H
#define WOOL0_RUNTIME_H

#include <pond_types.h>
#include <wool0.h>

#if (PRINT_TASK_STATS != 1)
#define wool0_task_inc_lcl(q)
#endif
//#ifndef __POND_POOL__
//#define wool0_update_dq_top(__dq_top)
//#endif

// Task definition for arity 0

#define TASK_PROTO_0(RTYPE, NAME ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top);                             \
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top);                                    \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_0(RTYPE, NAME ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
    } a;                                                              \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top);*/                                \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top)                              \
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
                                                                      \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top );                                 \
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top );                                   \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top );                                   \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top)                                     \
 
 

// Task definition for arity 1

#define TASK_PROTO_1(RTYPE, NAME, ATYPE_1, ARG_1 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1);              \
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1);                     \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_1(RTYPE, NAME, ATYPE_1, ARG_1 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1);*/                 \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1)               \
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1 );                   \
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1 );                     \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1 );                     \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1)                      \
 
 

// Task definition for arity 2

#define TASK_PROTO_2(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_2(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);*/  \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );     \
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );       \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );       \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2)       \
 
 

// Task definition for arity 3

#define TASK_PROTO_3(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_3(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\
 
 

// Task definition for arity 4

#define TASK_PROTO_4(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_4(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\
 
 

// Task definition for arity 5

#define TASK_PROTO_5(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_5(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\
 
 

// Task definition for arity 6

#define TASK_PROTO_6(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_6(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\
 
 

// Task definition for arity 7

#define TASK_PROTO_7(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_7(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\
 
 

// Task definition for arity 8

#define TASK_PROTO_8(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_8(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\
 
 

// Task definition for arity 9

#define TASK_PROTO_9(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_9(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\
 
 

// Task definition for arity 10

#define TASK_PROTO_10(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_10(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\
 
 

// Task definition for arity 11

#define TASK_PROTO_11(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_11(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\
 
 

// Task definition for arity 12

#define TASK_PROTO_12(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_12(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\
 
 

// Task definition for arity 13

#define TASK_PROTO_13(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_13(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\
 
 

// Task definition for arity 14

#define TASK_PROTO_14(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_14(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\
 
 

// Task definition for arity 15

#define TASK_PROTO_15(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_15(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\
 
 

// Task definition for arity 16

#define TASK_PROTO_16(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_16(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\
 
 

// Task definition for arity 17

#define TASK_PROTO_17(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_17(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\
 
 

// Task definition for arity 18

#define TASK_PROTO_18(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_18(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\
 
 

// Task definition for arity 19

#define TASK_PROTO_19(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_19(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	      ATYPE_19 ARG_19;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  p->d.a.ARG_19 = ARG_19;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19)\
 
 

// Task definition for arity 20

#define TASK_PROTO_20(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19, ATYPE_20, ARG_20 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);\
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
 
 
#define TASK_20(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19, ATYPE_20, ARG_20 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	      ATYPE_19 ARG_19;                                               \
	      ATYPE_20 ARG_20;                                               \
	    } a;                                                             \
    RTYPE res;                                                        \
	                                                                     \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  p->d.a.ARG_19 = ARG_19;                                             \
  p->d.a.ARG_20 = ARG_20;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
  t->d.res = CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
}                                                                     \
                                                                      \
RTYPE SYNC_##NAME(Task *__dq_top)                                     \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ( (TD_##NAME *) q )->d.res;                                \
  }                                                                   \
}                                                                     \
                                                                      \
RTYPE CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20)\
 
 

// Task definition for arity 0

#define VOID_TASK_PROTO_0(NAME ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top);                             \
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top);                                     \
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_0(NAME ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
    } a;                                                              \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top);*/                                 \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top)                              \
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
                                                                      \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top );                                           \
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top );                                   \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top );                                   \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top)                                      \
 
 

// Task definition for arity 1

#define VOID_TASK_PROTO_1(NAME, ATYPE_1, ARG_1 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1);              \
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1);                      \
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_1(NAME, ATYPE_1, ARG_1 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1);*/                  \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1)               \
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1 );                             \
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1 );                     \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1 );                     \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1)                       \
 
 

// Task definition for arity 2

#define VOID_TASK_PROTO_2(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);       \
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_2(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2);*/   \
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );               \
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );       \
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2 );       \
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2)        \
 
 

// Task definition for arity 3

#define VOID_TASK_PROTO_3(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_3(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 ); \
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\
 
 

// Task definition for arity 4

#define VOID_TASK_PROTO_4(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_4(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\
 
 

// Task definition for arity 5

#define VOID_TASK_PROTO_5(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_5(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\
 
 

// Task definition for arity 6

#define VOID_TASK_PROTO_6(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_6(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\
 
 

// Task definition for arity 7

#define VOID_TASK_PROTO_7(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_7(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\
 
 

// Task definition for arity 8

#define VOID_TASK_PROTO_8(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_8(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\
 
 

// Task definition for arity 9

#define VOID_TASK_PROTO_9(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_9(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\
 
 

// Task definition for arity 10

#define VOID_TASK_PROTO_10(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_10(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\
 
 

// Task definition for arity 11

#define VOID_TASK_PROTO_11(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_11(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\
 
 

// Task definition for arity 12

#define VOID_TASK_PROTO_12(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_12(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\
 
 

// Task definition for arity 13

#define VOID_TASK_PROTO_13(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_13(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\
 
 

// Task definition for arity 14

#define VOID_TASK_PROTO_14(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_14(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\
 
 

// Task definition for arity 15

#define VOID_TASK_PROTO_15(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_15(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\
 
 

// Task definition for arity 16

#define VOID_TASK_PROTO_16(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_16(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\
 
 

// Task definition for arity 17

#define VOID_TASK_PROTO_17(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_17(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\
 
 

// Task definition for arity 18

#define VOID_TASK_PROTO_18(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_18(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\
 
 

// Task definition for arity 19

#define VOID_TASK_PROTO_19(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_19(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	      ATYPE_19 ARG_19;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  p->d.a.ARG_19 = ARG_19;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19)\
 
 

// Task definition for arity 20

#define VOID_TASK_PROTO_20(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19, ATYPE_20, ARG_20 ) \
                                                                      \
typedef struct _TD_##NAME TD_##NAME;                                  \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);\
static void WRAP_##NAME(Task *, TD_##NAME *);                         \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);\
void SYNC_##NAME(Task *__dq_top)                                      \
 
 
#define VOID_TASK_20(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18, ATYPE_19, ARG_19, ATYPE_20, ARG_20 ) \
                                                                      \
struct _TD_##NAME {                                                   \
  TASK_COMMON_FIELDS( struct _TD_##NAME * )                           \
  union {                                                             \
    struct {                                                          \
      ATYPE_1 ARG_1;                                                  \
	      ATYPE_2 ARG_2;                                                 \
	      ATYPE_3 ARG_3;                                                 \
	      ATYPE_4 ARG_4;                                                 \
	      ATYPE_5 ARG_5;                                                 \
	      ATYPE_6 ARG_6;                                                 \
	      ATYPE_7 ARG_7;                                                 \
	      ATYPE_8 ARG_8;                                                 \
	      ATYPE_9 ARG_9;                                                 \
	      ATYPE_10 ARG_10;                                               \
	      ATYPE_11 ARG_11;                                               \
	      ATYPE_12 ARG_12;                                               \
	      ATYPE_13 ARG_13;                                               \
	      ATYPE_14 ARG_14;                                               \
	      ATYPE_15 ARG_15;                                               \
	      ATYPE_16 ARG_16;                                               \
	      ATYPE_17 ARG_17;                                               \
	      ATYPE_18 ARG_18;                                               \
	      ATYPE_19 ARG_19;                                               \
	      ATYPE_20 ARG_20;                                               \
	    } a;                                                             \
                                                                      \
  } d;                                                                \
};                                                                    \
                                                                      \
/*static void WRAP_##NAME(Task *, TD_##NAME *);*/                     \
/*void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20);*/\
                                                                      \
static void SPAWN_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20)\
{                                                                     \
  wool0_update_dq_top(__dq_top);                                       \
  TD_##NAME *p = (TD_##NAME *) __dq_top;                              \
  p->d.a.ARG_1 = ARG_1;                                               \
  p->d.a.ARG_2 = ARG_2;                                               \
  p->d.a.ARG_3 = ARG_3;                                               \
  p->d.a.ARG_4 = ARG_4;                                               \
  p->d.a.ARG_5 = ARG_5;                                               \
  p->d.a.ARG_6 = ARG_6;                                               \
  p->d.a.ARG_7 = ARG_7;                                               \
  p->d.a.ARG_8 = ARG_8;                                               \
  p->d.a.ARG_9 = ARG_9;                                               \
  p->d.a.ARG_10 = ARG_10;                                             \
  p->d.a.ARG_11 = ARG_11;                                             \
  p->d.a.ARG_12 = ARG_12;                                             \
  p->d.a.ARG_13 = ARG_13;                                             \
  p->d.a.ARG_14 = ARG_14;                                             \
  p->d.a.ARG_15 = ARG_15;                                             \
  p->d.a.ARG_16 = ARG_16;                                             \
  p->d.a.ARG_17 = ARG_17;                                             \
  p->d.a.ARG_18 = ARG_18;                                             \
  p->d.a.ARG_19 = ARG_19;                                             \
  p->d.a.ARG_20 = ARG_20;                                             \
  if( p->stealable ) {                                                \
    SFENCE;                                                           \
  }                                                                   \
  p->f     = &WRAP_##NAME;                                            \
}                                                                     \
                                                                      \
static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)                 \
{                                                                     \
   CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
}                                                                     \
                                                                      \
void SYNC_##NAME(Task *__dq_top)                                      \
{                                                                     \
  Task *q = __dq_top;                                                 \
  balarm_t a;                                                         \
                                                                      \
  if( ! q->stealable ) {                                              \
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
  }                                                                   \
  q->f = T_BUSY;                                                      \
  MFENCE;                                                             \
  a = q->balarm;                                                      \
                                                                      \
  if( a == NOT_STOLEN || ( a = __wool0_sync_get_balarm( q ) ) == NOT_STOLEN ) {\
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */     \
    /* Not stolen, nobody else might be using it */                   \
	wool0_update_dq_top(q);                                               \
	wool0_task_inc_lcl(q);                                                \
    return CALL_##NAME( __dq_top, t->d.a.ARG_1, t->d.a.ARG_2, t->d.a.ARG_3, t->d.a.ARG_4, t->d.a.ARG_5, t->d.a.ARG_6, t->d.a.ARG_7, t->d.a.ARG_8, t->d.a.ARG_9, t->d.a.ARG_10, t->d.a.ARG_11, t->d.a.ARG_12, t->d.a.ARG_13, t->d.a.ARG_14, t->d.a.ARG_15, t->d.a.ARG_16, t->d.a.ARG_17, t->d.a.ARG_18, t->d.a.ARG_19, t->d.a.ARG_20 );\
  } else {                                                            \
    __wool0_SYNC( __dq_top, a );                                       \
	wool0_update_dq_top(__dq_top);                                        \
    return ;                                                          \
  }                                                                   \
}                                                                     \
                                                                      \
void CALL_##NAME(Task *__dq_top, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18, ATYPE_19 ARG_19, ATYPE_20 ARG_20)\
 
 
	#define LOOP_BODY_PROTO_0(NAME, COST, IXTY, IXNAME)                  \
		VOID_TASK_PROTO_2(TREE_##NAME, IXTY, __from, IXTY, __to);           \
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME)                       \
 
 
	#define LOOP_BODY_0(NAME, COST, IXTY, IXNAME)                        \
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME);*/            \
                                                                      \
	VOID_TASK_2(TREE_##NAME, IXTY, __from, IXTY, __to)                   \
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i );                                       \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to );                                  \
		CALL( TREE_##NAME, __from, __mid );                                 \
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME)                        \


	#define LOOP_BODY_PROTO_1(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1)  \
		VOID_TASK_PROTO_3(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1)        \
 
 
	#define LOOP_BODY_1(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1)        \
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1);*/\
                                                                      \
	VOID_TASK_3(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1)   \
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1 );                                \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1 );                           \
		CALL( TREE_##NAME, __from, __mid, ARG_1 );                          \
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1)         \


	#define LOOP_BODY_PROTO_2(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)\
		VOID_TASK_PROTO_4(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2)\
 
 
	#define LOOP_BODY_2(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2);*/\
                                                                      \
	VOID_TASK_4(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2 );                         \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2 );                    \
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2 );                   \
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2)\


	#define LOOP_BODY_PROTO_3(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)\
		VOID_TASK_PROTO_5(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\
 
 
	#define LOOP_BODY_3(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3);*/\
                                                                      \
	VOID_TASK_5(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3 );                  \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3 );             \
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3 );            \
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\


	#define LOOP_BODY_PROTO_4(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)\
		VOID_TASK_PROTO_6(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\
 
 
	#define LOOP_BODY_4(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4);*/\
                                                                      \
	VOID_TASK_6(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4 );           \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4 );      \
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4 );     \
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\


	#define LOOP_BODY_PROTO_5(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)\
		VOID_TASK_PROTO_7(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\
 
 
	#define LOOP_BODY_5(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5);*/\
                                                                      \
	VOID_TASK_7(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5 );    \
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\


	#define LOOP_BODY_PROTO_6(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)\
		VOID_TASK_PROTO_8(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\
 
 
	#define LOOP_BODY_6(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6);*/\
                                                                      \
	VOID_TASK_8(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\


	#define LOOP_BODY_PROTO_7(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7)\
		VOID_TASK_PROTO_9(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\
 
 
	#define LOOP_BODY_7(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7);*/\
                                                                      \
	VOID_TASK_9(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7)\


	#define LOOP_BODY_PROTO_8(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8)\
		VOID_TASK_PROTO_10(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\
 
 
	#define LOOP_BODY_8(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8);*/\
                                                                      \
	VOID_TASK_10(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8)\


	#define LOOP_BODY_PROTO_9(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9)\
		VOID_TASK_PROTO_11(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\
 
 
	#define LOOP_BODY_9(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9);*/\
                                                                      \
	VOID_TASK_11(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9)\


	#define LOOP_BODY_PROTO_10(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10)\
		VOID_TASK_PROTO_12(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\
 
 
	#define LOOP_BODY_10(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10);*/\
                                                                      \
	VOID_TASK_12(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10)\


	#define LOOP_BODY_PROTO_11(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11)\
		VOID_TASK_PROTO_13(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\
 
 
	#define LOOP_BODY_11(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11);*/\
                                                                      \
	VOID_TASK_13(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11)\


	#define LOOP_BODY_PROTO_12(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12)\
		VOID_TASK_PROTO_14(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\
 
 
	#define LOOP_BODY_12(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12);*/\
                                                                      \
	VOID_TASK_14(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12)\


	#define LOOP_BODY_PROTO_13(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13)\
		VOID_TASK_PROTO_15(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\
 
 
	#define LOOP_BODY_13(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13);*/\
                                                                      \
	VOID_TASK_15(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13)\


	#define LOOP_BODY_PROTO_14(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14)\
		VOID_TASK_PROTO_16(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\
 
 
	#define LOOP_BODY_14(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14);*/\
                                                                      \
	VOID_TASK_16(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14)\


	#define LOOP_BODY_PROTO_15(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15)\
		VOID_TASK_PROTO_17(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\
 
 
	#define LOOP_BODY_15(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15);*/\
                                                                      \
	VOID_TASK_17(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15)\


	#define LOOP_BODY_PROTO_16(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16)\
		VOID_TASK_PROTO_18(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\
 
 
	#define LOOP_BODY_16(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16);*/\
                                                                      \
	VOID_TASK_18(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16)\


	#define LOOP_BODY_PROTO_17(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17)\
		VOID_TASK_PROTO_19(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\
 
 
	#define LOOP_BODY_17(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17);*/\
                                                                      \
	VOID_TASK_19(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17)\


	#define LOOP_BODY_PROTO_18(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18)\
		VOID_TASK_PROTO_20(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18);\
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\
 
 
	#define LOOP_BODY_18(NAME, COST, IXTY, IXNAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18)\
                                                                      \
	static unsigned long const __min_iters__##NAME                       \
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );     \
                                                                      \
	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18);*/\
                                                                      \
	VOID_TASK_20(TREE_##NAME, IXTY, __from, IXTY, __to, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6, ATYPE_7, ARG_7, ATYPE_8, ARG_8, ATYPE_9, ARG_9, ATYPE_10, ARG_10, ATYPE_11, ARG_11, ATYPE_12, ARG_12, ATYPE_13, ARG_13, ATYPE_14, ARG_14, ATYPE_15, ARG_15, ATYPE_16, ARG_16, ATYPE_17, ARG_17, ATYPE_18, ARG_18)\
	{                                                                    \
	if( __to - __from <= __min_iters__##NAME ) {                         \
		IXTY __i;                                                           \
		for( __i = __from; __i < __to; __i++ ) {                            \
		LOOP_##NAME( __dq_top, __i, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17, ARG_18 );\
		}                                                                   \
	} else {                                                             \
		IXTY __mid = (__from + __to) / 2;                                   \
		SPAWN( TREE_##NAME, __mid, __to, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17, ARG_18 );\
		CALL( TREE_##NAME, __from, __mid, ARG_1, ARG_2, ARG_3, ARG_4, ARG_5, ARG_6, ARG_7, ARG_8, ARG_9, ARG_10, ARG_11, ARG_12, ARG_13, ARG_14, ARG_15, ARG_16, ARG_17, ARG_18 );\
		SYNC( TREE_##NAME );                                                \
	}                                                                    \
	}                                                                    \
                                                                      \
	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME, ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6, ATYPE_7 ARG_7, ATYPE_8 ARG_8, ATYPE_9 ARG_9, ATYPE_10 ARG_10, ATYPE_11 ARG_11, ATYPE_12 ARG_12, ATYPE_13 ARG_13, ATYPE_14 ARG_14, ATYPE_15 ARG_15, ATYPE_16 ARG_16, ATYPE_17 ARG_17, ATYPE_18 ARG_18)\

#endif /* WOOL0_RUNTIME_H */

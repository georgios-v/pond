#! /bin/bash

# ./wool_root.sh -t 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 -v 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 -l 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 > runtime.h
# Copyright notice:
echo "
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
" > /dev/null

args=`getopt -o t:v:l: -- "$@"`
eval set -- "$args"
task='';
vtask='';
loop='';
for i do
	case "$i" in
		-t) shift;task=$(echo $1|sed 's/,/ /g');shift;;
		-v) shift;vtask=$(echo $1|sed 's/,/ /g');shift;;
		-l) shift;loop=$(echo $1|sed 's/,/ /g');shift;;
	esac
done

arity_max=0;
for i in $task; do
	if [ "$i" -gt "$arity_max" ]; then
		arity_max=$i;
	fi;
done;
for i in $vtask; do
	if [ "$i" -gt "$arity_max" ]; then
		arity_max=$i;
	fi
done;
for i in $loop; do
	if [ "$i" -gt "$arity_max" ]; then
		arity_max=$i;
	fi
done;

function protobody {
(\
echo "
typedef struct _TD_##NAME TD_##NAME;
static void SPAWN_##NAME(Task *__dq_top$TASK_ARGS);
static void WRAP_##NAME(Task *, TD_##NAME *);
$RTYPE CALL_##NAME(Task *__dq_top$TASK_ARGS);
$RTYPE SYNC_##NAME(Task *__dq_top)" \
) | awk '{printf "%-70s\\\n", $0 }'
}


function taskbody {
(\
echo "
struct _TD_##NAME {
  TASK_COMMON_FIELDS( struct _TD_##NAME * )
  union {
    struct {
$FIELD_DECL\
    } a;
    $RES_FIELD
  } d;
};

/*static void WRAP_##NAME(Task *, TD_##NAME *);*/
/*$RTYPE CALL_##NAME(Task *__dq_top$TASK_ARGS);*/

static void SPAWN_##NAME(Task *__dq_top$TASK_ARGS)
{
  wool_update_dq_top(__dq_top);
  TD_##NAME *p = (TD_##NAME *) __dq_top;
$TASK_INIT
  if( p->stealable ) {
    SFENCE;
  }
  p->f     = &WRAP_##NAME;
}

static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)
{
  $SAVE_RVAL CALL_##NAME( __dq_top$TASK_GET_FROM_t );
}

$RTYPE SYNC_##NAME(Task *__dq_top)
{
  Task *q = __dq_top;
  balarm_t a;

  if( ! q->stealable ) {
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */
	wool_update_dq_top(q);
	wool_task_inc_lcl(q);
    return CALL_##NAME( __dq_top$TASK_GET_FROM_t );
  }
  q->f = T_BUSY;
  MFENCE;
  a = q->balarm;

  if( a == NOT_STOLEN || ( a = __wool_sync_get_balarm( q ) ) == NOT_STOLEN ) {
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */
    /* Not stolen, nobody else might be using it */
	wool_update_dq_top(q);
	wool_task_inc_lcl(q);
    return CALL_##NAME( __dq_top$TASK_GET_FROM_t );
  } else {
    __wool_SYNC( __dq_top, a );
	wool_update_dq_top(__dq_top);
    return $RETURN_RES;
  }
}

$RTYPE CALL_##NAME(Task *__dq_top$TASK_ARGS)" \
) | awk '{printf "%-70s\\\n", $0 }'
}

#
# Second part, once for each arity
#

for r in $task; do

	MACRO_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		MACRO_ARGS="$MACRO_ARGS, ATYPE_$i, ARG_$i"
	done

	FIELD_DECL=""
	for(( i = 1; i <= $r; i++ )) do
		FIELD_DECL="${FIELD_DECL}      ATYPE_$i ARG_$i;
	"
	done

	TASK_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		TASK_ARGS="$TASK_ARGS, ATYPE_$i ARG_$i"
	done

	TASK_INIT=`for(( i = 1; i <= $r; i++ )) do
		echo "  p->d.a.ARG_$i = ARG_$i;"
	done;`

	TASK_GET_FROM_t=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", t->d.a.ARG_$i"
	done`

	CALL_ARGS=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", ARG_$i"
	done`
	RTYPE="RTYPE";
	RES_FIELD="$RTYPE res;
	";
	SAVE_RVAL="t->d.res =";
	RETURN_RES="( (TD_##NAME *) q )->d.res";
	ASSIGN_RES="res = ";
	RES_VAR="res";

	echo
	echo "// Task definition for arity $r"
	echo
	echo "#define TASK_PROTO_$r(RTYPE, NAME$MACRO_ARGS ) \\";
	protobody;
	echo " ";
	echo " ";
	echo "#define TASK_$r(RTYPE, NAME$MACRO_ARGS ) \\";
	taskbody;

	echo " ";
	echo " ";
done


for r in $vtask; do

	MACRO_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		MACRO_ARGS="$MACRO_ARGS, ATYPE_$i, ARG_$i"
	done

	FIELD_DECL=""
	for(( i = 1; i <= $r; i++ )) do
		FIELD_DECL="${FIELD_DECL}      ATYPE_$i ARG_$i;
	"
	done

	TASK_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		TASK_ARGS="$TASK_ARGS, ATYPE_$i ARG_$i"
	done

	TASK_INIT=`for(( i = 1; i <= $r; i++ )) do
		echo "  p->d.a.ARG_$i = ARG_$i;"
	done;`

	TASK_GET_FROM_t=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", t->d.a.ARG_$i"
	done`

	CALL_ARGS=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", ARG_$i"
	done`
	RTYPE="void"
	RES_FIELD=""
	SAVE_RVAL=""
	RETURN_RES=""
	ASSIGN_RES=""
	RES_VAR=""

	echo
	echo "// Task definition for arity $r"
	echo
	echo "#define VOID_TASK_PROTO_$r(NAME$MACRO_ARGS ) \\";
	protobody;
	echo " ";
	echo " ";
	echo "#define VOID_TASK_$r(NAME$MACRO_ARGS ) \\";
	taskbody;
	echo " ";
	echo " ";
done


for r in $loop; do

	MACRO_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		MACRO_ARGS="$MACRO_ARGS, ATYPE_$i, ARG_$i"
	done

	FIELD_DECL=""
	for(( i = 1; i <= $r; i++ )) do
		FIELD_DECL="${FIELD_DECL}      ATYPE_$i ARG_$i;
	"
	done

	TASK_ARGS=""
	for(( i = 1; i <= $r; i++ )) do
		TASK_ARGS="$TASK_ARGS, ATYPE_$i ARG_$i"
	done

	TASK_INIT=`for(( i = 1; i <= $r; i++ )) do
		echo "  p->d.a.ARG_$i = ARG_$i;"
	done;`

	TASK_GET_FROM_t=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", t->d.a.ARG_$i"
	done`

	CALL_ARGS=`for(( i = 1; i <= $r; i++ )) do
		echo -n ", ARG_$i"
	done`

	(\
	echo "\
	#define LOOP_BODY_PROTO_$r(NAME, COST, IXTY, IXNAME$MACRO_ARGS)
		VOID_TASK_PROTO_$((r+2))(TREE_##NAME, IXTY, __from, IXTY, __to$MACRO_ARGS);
		void LOOP_##NAME(Task *__dq_top, IXTY IXNAME$TASK_ARGS)" \
	) | awk '{printf "%-70s\\\n", $0 }';
	echo " ";
	echo " ";
	(\
	echo "\
	#define LOOP_BODY_$r(NAME, COST, IXTY, IXNAME$MACRO_ARGS)

	static unsigned long const __min_iters__##NAME
	= COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );

	/*inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME$TASK_ARGS);*/

	VOID_TASK_$((r+2))(TREE_##NAME, IXTY, __from, IXTY, __to$MACRO_ARGS)
	{
	if( __to - __from <= __min_iters__##NAME ) {
		IXTY __i;
		for( __i = __from; __i < __to; __i++ ) {
		LOOP_##NAME( __dq_top, __i$CALL_ARGS );
		}
	} else {
		IXTY __mid = (__from + __to) / 2;
		SPAWN( TREE_##NAME, __mid, __to$CALL_ARGS );
		CALL( TREE_##NAME, __from, __mid$CALL_ARGS );
		SYNC( TREE_##NAME );
	}
	}

	void LOOP_##NAME(Task *__dq_top, IXTY IXNAME$TASK_ARGS)" \
	) | awk '{printf "%-70s\\\n", $0 }';
	echo -e "\n";
done

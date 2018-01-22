/*
 * File:   topology_gothmog.h
 * Author: Georgios Varisteas
 *
 * Created on February 5, 2014, 10:58 AM
 */

#ifndef TOPOLOGY_I5_H
#define	TOPOLOGY_I5_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef TOPOLOGY_CORE_MASTER
#define TOPOLOGY_CORE_MASTER 0
#endif

#ifndef TOPOLOGY_CORE_HELPER
#define TOPOLOGY_CORE_HELPER 0
#endif

#ifndef TOPOLOGY_CORE_FIRST
#define TOPOLOGY_CORE_FIRST 1
#endif

#ifndef TOPOLOGY_CORE_LAST
#define TOPOLOGY_CORE_LAST 3
#endif

#ifndef TOPOLOGY_CORE_COUNT
#define TOPOLOGY_CORE_COUNT 3
#endif

#ifndef TOPOLOGY_NODE_COUNT
#define TOPOLOGY_NODE_COUNT 1
#endif

#ifndef TOPOLOGY_CORES_PER_NODE_COUNT
#define TOPOLOGY_CORES_PER_NODE_COUNT 4
#endif

#ifndef TOPOLOGY_MAXIMUM_DISTANCE
#define TOPOLOGY_MAXIMUM_DISTANCE 1
#endif

#ifndef SCHED_CORE_ASSIGNED_THRESHOLD
#define SCHED_CORE_ASSIGNED_THRESHOLD 1
#endif

#define TOPOLOGY_DEFINED

#ifdef	__cplusplus
}
#endif

#endif	/* TOPOLOGY_I5_H */


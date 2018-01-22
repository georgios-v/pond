/*
 * File:   topology_gothmog.h
 * Author: Georgios Varisteas
 *
 * Created on February 5, 2014, 10:58 AM
 */

#ifndef TOPOLOGY_GOTHMOG_H
#define	TOPOLOGY_GOTHMOG_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef TOPOLOGY_CORE_MASTER
#define TOPOLOGY_CORE_MASTER 0
#endif

#ifndef TOPOLOGY_CORE_HELPER
#define TOPOLOGY_CORE_HELPER 1
#endif

#ifndef TOPOLOGY_CORE_FIRST
#define TOPOLOGY_CORE_FIRST 2
#endif

#ifndef TOPOLOGY_CORE_LAST
#define TOPOLOGY_CORE_LAST 55
#endif

#ifndef TOPOLOGY_CORE_COUNT
#define TOPOLOGY_CORE_COUNT 55
#endif

#ifndef TOPOLOGY_NODE_COUNT
#define TOPOLOGY_NODE_COUNT 1
#endif

#ifndef TOPOLOGY_CORES_PER_NODE_COUNT
#define TOPOLOGY_CORES_PER_NODE_COUNT 56
#endif

#ifndef TOPOLOGY_MAXIMUM_DISTANCE
#define TOPOLOGY_MAXIMUM_DISTANCE 13
#endif

#ifndef SCHED_CORE_ASSIGNED_THRESHOLD
#define SCHED_CORE_ASSIGNED_THRESHOLD 1
#endif

#define TOPOLOGY_DEFINED

#ifdef	__cplusplus
}
#endif

#endif	/* TOPOLOGY_GOTHMOG_H */


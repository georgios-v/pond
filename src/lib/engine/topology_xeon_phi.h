/*
 * File:   topology_xeon_phi.h
 * Author: Georgios Varisteas
 *
 * Created on February 6, 2014, 1:56 PM
 */

#ifndef TOPOLOGY_XEON_PHI_H
#define	TOPOLOGY_XEON_PHI_H

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
#define TOPOLOGY_CORE_LAST 227
#endif

#ifndef TOPOLOGY_CORE_COUNT
#define TOPOLOGY_CORE_COUNT 228
#endif

#ifndef TOPOLOGY_NODE_COUNT
#define TOPOLOGY_NODE_COUNT 57
#endif

#ifndef TOPOLOGY_CORES_PER_NODE_COUNT
#define TOPOLOGY_CORES_PER_NODE_COUNT 4
#endif

#ifndef TOPOLOGY_MAXIMUM_DISTANCE
#define TOPOLOGY_MAXIMUM_DISTANCE 29
#endif

#ifndef SCHED_CORE_ASSIGNED_THRESHOLD
#define SCHED_CORE_ASSIGNED_THRESHOLD 1
#endif

#define TOPOLOGY_DEFINED

#ifdef	__cplusplus
}
#endif

#endif	/* TOPOLOGY_XEON_PHI_H */


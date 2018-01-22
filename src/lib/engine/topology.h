/*
 * File:   topology.h
 * Author: Georgios Varisteas
 *
 * Created on January 27, 2014, 9:42 AM
 */

#ifndef TOPOLOGY_H
#define	TOPOLOGY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <pond_types.h>
#include <stdbool.h>
#include "bitmap.h"
	
	typedef struct core {
		bool sourced:1;				///< true if this is a source
		coreid_t assignments:11;	///< number of sources this cores is assigned to
		coreid_t id:10;				///< core physical id.
		coreid_t idx:10;			///< core index. Since some cores are not available, this is the topology_cores and _threads
		mutex_t lock;
	} core_t;


#if defined(__GOTHMOG__)
#include "topology_gothmog.h"
#elif defined(__TILERA__)
#include "topology_tilera.h"
#elif defined(__XEON_PHI__)
#include "topology_xeon_phi.h"
#elif defined(__i7__)
#include "topology_i7.h"
#elif defined(__i5__)
#include "topology_i5.h"
#endif

#ifndef TOPOLOGY_DEFINED
#error Topology definition required
#endif


	void topology_cores_init(void);
	/**
	 * Get the distance between two cores
     * @param a	The first core
     * @param b The second core
     * @return return an integer value of the distance between two cores
     */
	coreid_t topology_get_distance_of_cores(coreid_t a, coreid_t b);
	/**
	 * Get the collection of cores located at a specific distance from a central core
	 * @NOTE	This function should allocate the buffer accordingly
     * @param core	the central core
     * @param dist	the desired distance
     * @param buffer	a pointer to an unallocated buffer to keep the cores
     * @return	the integer count of the cores placed in the buffer
     */
	coreid_t topology_get_cores_at_distance(coreid_t core, coreid_t dist, coreid_t **buffer);
	/**
	 * Get the core_t object given the core's external id (provided and recognizable by the OS)
	 * @NOTE This function makes no check on the existence of the core.
	 * @param id	the id code of the desired core
	 * @return the respective core object
	 */
	core_t *topology_get_core_from_id(coreid_t id);

	/**
	 * Get the core_t object given the core's internal index (not recognizable by the OS)
	 * @NOTE This function makes no check on the existence of the core.
	 * @param idx	the index of the desired core
	 * @return the respective core object
	 */
	core_t *topology_get_core_from_idx(coreid_t idx);
#ifdef	__cplusplus
}
#endif

#endif	/* TOPOLOGY_H */


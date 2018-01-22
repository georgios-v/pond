/*
 * File:   dvs.h
 * Author: Georgios Varisteas
 *
 * Created on March 10, 2014, 1:53 PM
 */

#ifndef DVS_H
#define	DVS_H

#ifdef	__cplusplus
extern "C" {
#endif

///< the maximum size of the VO set is always 4 times the dimensions + 4 (2 + self + terminator)
#define MAX_VO 7
	
#ifndef _USR_DEBUG_DVS
#define _USR_DEBUG_DVS 0
#endif

void dvs_init_island(opt_allotment_t allot);
coreid_t dvs_find_d(coreid_t count);

#ifdef	__cplusplus
}
#endif

#endif	/* DVS_H */


/* 
 * File:   bindings.h
 * Author: georgios
 *
 * Created on January 20, 2013, 11:10 AM
 */

#ifndef POND_BINDINGS_H
#define	POND_BINDINGS_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef BARRRELFISH
	program_t get_program_to_init(void);
	program_t get_program_to_fini(coreid_t sid);
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* POND_BINDINGS_H */


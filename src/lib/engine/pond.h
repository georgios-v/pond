/* 
 * File:   pond.h
 * Author: georgios
 *
 * Created on February 7, 2014, 4:23 PM
 */

#ifndef POND_H
#define	POND_H

#ifdef	__cplusplus
extern "C" {
#endif

int execute_program_custom(struct __pond_app *app, char runtime, int source_id, coreid_t core_count, 
		uint64_t helper_sleep, int argc, char **argv, int argv_offset);


#ifdef	__cplusplus
}
#endif

#endif	/* POND_H */


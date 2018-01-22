/* 
 * File:   definitions.h
 * Author: georgios
 *
 * Created on February 7, 2014, 4:17 PM
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

///< set to 0 to globally supress verbose output
#ifndef _SYS_VERBOSE
#define _SYS_VERBOSE _SYS_VERBOSE_GLOBAL
#endif
///< set to 0 to globally supress debug output
#ifndef _SYS_DEBUG
#define _SYS_DEBUG _SYS_DEBUG_GLOBAL
#endif

#ifndef BARRELFISH
#include <stdlib.h>
#include <stdio.h>
void debug_printf(FILE *__restrict __stream, const char *fmt, ...);
#endif

/// normal print
#define print(...)  debug_printf(stdout, __VA_ARGS__)
/// error print
#define error(...)  debug_printf(stderr, __VA_ARGS__)
/// normal print
#define cprint(__condition, ...)  if(__condition) {print(__VA_ARGS__);}
/// error print
#define cerror(__condition, ...)  if(__condition) {error(__VA_ARGS__);}
/// Verbose print
#define verbose(...)  if(_SYS_VERBOSE) {print(__VA_ARGS__);}
/// Debug print
#define debug(...)  if(_SYS_DEBUG) {error(__VA_ARGS__);}

#ifdef	__cplusplus
}
#endif

#endif	/* DEFINITIONS_H */


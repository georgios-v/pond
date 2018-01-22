/* 
 * File:   config.h
 * Author: Georgios Varisteas
 *
 * Created on December 12, 2011, 11:19 AM
 */

#ifndef BOTS_CONFIG_H
#define	BOTS_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include "runtime0.h"

#ifdef BARRELFISH
#define alloca(size) __builtin_alloca(size)
#endif

#define BOTS_SEQUENTIAL false
#define BOTS_CONFIG		false

#endif	/* BOTS_CONFIG_H */

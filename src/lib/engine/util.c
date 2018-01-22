/*
 * File:   util.c
 * Author: Georgios Varisteas
 *
 * Created on October 4, 2012, 19:04 AM
 */


#ifndef BARRELFISH
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "topology.h"

#include <sched.h>
#include <pthread_compat.h>
#include <errno.h>

extern core_t topology_cores[]; ///< Number of available cores

int setaffinity(pthread_t thread, unsigned short int c) {
	cpu_set_t *csp = NULL;
	csp = CPU_ALLOC(1);
	if (csp == NULL) {
		perror("CPU_ALLOC");
		exit(EXIT_FAILURE);
	}
	size_t size = CPU_ALLOC_SIZE(1);
	CPU_ZERO_S(size, csp);
	/* Set affinity mask to include just CPU c */
	CPU_SET_S(c, size, csp);
	int ret = pthread_setaffinity_np(thread, size, csp);
	CPU_FREE(csp);
	return ret;
}

int getaffinity(pthread_t thread) {
	int j = 0, ret = -1;
	cpu_set_t *csp = NULL;
	csp = CPU_ALLOC(1);
	size_t size = CPU_ALLOC_SIZE(1);
	pthread_getaffinity_np(thread, size, csp);
	for (j = 0; j <  TOPOLOGY_CORE_COUNT + TOPOLOGY_CORE_FIRST; j++)
		if (CPU_ISSET(j, csp)) {
			ret = j;
			break;
		}
	CPU_FREE(csp);
	return ret;
}

void debug_printf(FILE *__restrict __stream, const char *fmt, ...)	{
	va_list argptr;
	char str[10240];
	size_t len = 0;

	len = snprintf(str, sizeof(str), "%d: ", getaffinity(pthread_self()));
	if (len < sizeof(str)) {
		va_start(argptr, fmt);
		vsnprintf(str + len, sizeof(str) - len, fmt, argptr);
		va_end(argptr);
	}
	fprintf(__stream, "%s", str);
}
#endif

#include <string.h>

char *itoa(int n, char *s, unsigned int b);
char *strrev(char *);

char *strrev(char *str) {
	char *p1, *p2;
	if (!str || !*str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

char *itoa(int n, char *s, unsigned int b) {
	static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i = 0, sign;
	if ((sign = n) < 0)
		n = -n;
	do {
		s[i++] = digits[n % b];
	} while ((n /= b) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\n';
	return strrev(s);
}

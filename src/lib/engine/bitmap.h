/* 
 * File:   bitmap.h
 * Author: georgios
 *
 * Created on March 11, 2014, 7:44 PM
 */

#ifndef BITMAP_H
#define	BITMAP_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <limits.h>   /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */

	typedef uint64_t word_t;
	typedef word_t *bitmap;

#define BITS_PER_WORD (sizeof(word_t) * CHAR_BIT)

#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

	bitmap bitmap_init(int max_size);
	void bitmap_destroy(bitmap bm);
	void bitmap_set(bitmap bm, int n);
	void bitmap_clear(bitmap bm, int n);
	int bitmap_get(bitmap bm, int n);


#ifdef	__cplusplus
}
#endif

#endif	/* BITMAP_H */


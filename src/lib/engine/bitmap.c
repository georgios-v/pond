#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "bitmap.h"

bitmap bitmap_init(int max_size){
	bitmap bm = calloc((max_size / BITS_PER_WORD) + 1, sizeof(*bm));
	if(bm == NULL){
		error("ERROR: memory allocation error. bitmap.\n");
		exit(1);
	}
	return bm;
}

void bitmap_destroy(bitmap bm){
	free(bm);
}

void bitmap_set(bitmap bm, int n) {
	bm[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void bitmap_clear(bitmap bm, int n) {
	bm[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));
}

int bitmap_get(bitmap bm, int n) {
	word_t bit = bm[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
	return bit != 0;
}

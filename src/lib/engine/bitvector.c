/*
 * File:   bitvector.c
 * Author: Georgios Varisteas
 *
 * Created on February 3, 2014, 10:38 AM
 */
#include <stdlib.h>
#include "bitvector.h"

static uint64_t lg2(uint64_t n);


/**
 * Initialize a preallocated bitvector
 * @param bv: pointer to the preallocated bitvector structure. No member should be assigned
 * @param record_count: maximum number of records for the bitvector to hold
 */
void bitvector_init(bitvector_t *bv, uint record_count, uint16_t max_value) {
	bv->rec_mask = max_value;
	bv->length = lg2(max_value);
	bv->count = 64 / bv->length;
	bv->size = (record_count / bv->count) + 1;
	bv->data = malloc(sizeof(uint64_t)*bv->size);
}

/**
 * Free a preallocated bitvector
 * @param bv: pointer to the preallocated bitvector structure
 */
void bitvector_kill(bitvector_t *bv) {
	free(bv->data);
	free(bv);
}


static inline void bitvector_unpack(bitvector_t *bv, uint16_t *v, uint edx) {
	int i;
	// unpack edx entry
	for (i = 0; i < bv->count; i++) {
		v[i] = (bv->data[edx] >> (i * bv->length)) & bv->rec_mask;
	}
}


static inline void bitvector_pack(bitvector_t *bv, uint16_t *v, uint edx) {
	int i;
	// reset current entry
	bv->data[edx] = 0;
	// repack entry
	for (i = 0; i < bv->count; i++) {
		bv->data[edx] |= v[i] << (i * bv->length);
	}
}

/**
 * Set a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param n: data to write in the bitvector
 * @param pos: record's index
 */
void bitvector_set(bitvector_t *bv, uint16_t n, uint pos) {
	// the corresponding entry must be unpacked
	// the specific record updated
	// and the entry re-packed
	// create a buffer to unpack the entry in
	uint16_t v[bv->count];
	// find the entry that holds the record
	uint edx = pos / bv->count;
	// calculate relative record's index
	uint p = pos - (edx*bv->count);
	// unpack edx entry
	bitvector_unpack(bv, v, edx);
	// set new value
	v[p] = n;
	// repack entry
	bitvector_pack(bv, v, edx);
}

/**
 * Increment a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param n: value to increment by the record
 * @param pos: record's index
 */
void bitvector_inc(bitvector_t *bv, uint16_t n, uint pos) {
	// the corresponding entry must be unpacked
	// the specific record updated
	// and the entry re-packed
	// create a buffer to unpack the entry in
	uint16_t v[bv->count];
	// find the entry that holds the record
	uint edx = pos / bv->count;
	// calculate relative record's index
	uint p = pos - (edx*bv->count);
	// unpack edx entry
	bitvector_unpack(bv, v, edx);
	// set new value
	v[p] += n;
	// repack entry
	bitvector_pack(bv, v, edx);
}

/**
 * Decrement a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param n: value to decrement by the record
 * @param pos: record's index
 */
void bitvector_dec(bitvector_t *bv, uint16_t n, uint pos) {
	// the corresponding entry must be unpacked
	// the specific record updated
	// and the entry re-packed
	// create a buffer to unpack the entry in
	uint16_t v[bv->count];
	// find the entry that holds the record
	uint edx = pos / bv->count;
	// calculate relative record's index
	uint p = pos - (edx*bv->count);
	// unpack edx entry
	bitvector_unpack(bv, v, edx);
	// set new value
	v[p] -=n;
	// repack entry
	bitvector_pack(bv, v, edx);
}

/**
 * Get a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param pos: record's index
 */
uint16_t bitvector_get(bitvector_t *bv, uint pos) {
	// find the entry that holds the record
	uint edx = pos / bv->count;
	// calculate relative record's index
	uint p = pos - (edx*bv->count);
	// return p record from edx entry
	return (bv->data[edx] >> (p * bv->length)) & bv->rec_mask;
}

/**
 * Set a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param v: preallocated array of int records to write in the bitvector
 * @param edx: entry's index
 */
void bitvector_set_entry(bitvector_t *bv, uint16_t *v, uint edx) {
	// repack entry
	bitvector_pack(bv, v, edx);
}

/**
 * Get a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param v: pointer to a preallocated buffer to hold the unpacked int values
 * @param edx: entry's index
 */
void bitvector_get_entry(bitvector_t *bv, uint16_t *v, uint edx) {
	// unpack edx entry
	bitvector_unpack(bv, v, edx);
}

/**
 * Set a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param v: preallocated array of int records to write in the bitvector
 * @param edx: record's index
 */
void bitvector_set_entry_of_record(bitvector_t *bv, uint16_t *v, uint pos) {
	// find the entry that holds the record
	uint edx = pos / bv->count;
	bitvector_set_entry(bv, v, edx);
}

/**
 * Get a value in the bitvector.
 * @param bv: pointer to the bitvector structure. Must be initialized
 * @param v: pointer to a preallocated buffer to hold the unpacked int values
 * @param pos: record's index
 */
void bitvector_get_entry_of_record(bitvector_t *bv, uint16_t *v, uint pos) {
	// find the entry that holds the record
	uint edx = pos / bv->count;
	bitvector_get_entry(bv, v, edx);
}

/** get the number of bits of an positive integer n */
static uint64_t lg2(uint64_t n) {
	uint64_t i = 0;
	while (n) {
		i++;
		n >>= 1;
	}
	return i;
}

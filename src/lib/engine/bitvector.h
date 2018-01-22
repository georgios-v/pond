/*
 * File:   bitvector.h
 * Author: Georgios Varisteas
 *
 * Created on February 3, 2014, 10:38 AM
 */

#ifndef BITVECTOR_H
#define	BITVECTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

	typedef unsigned int uint;

	/** Bit storage. It allocates a number of X-bit entries
	 *   Each entry holds (size/length) number of records
	 *   Records do not span multiple entries
	 *   64 bits is the ideal storage size for all available architectures
	 *    in order to minimize wastefulness.
	 */
	typedef struct bitvector{
		int64_t *data;	///< entry storage
		uint size;		///< number of data array entries
		uint count;		///< number of records per entry
		uint length;	///< number of bits per actual record
		uint rec_mask;	///< record-sized binary mask (<length> number of 1 bits)
	} bitvector_t;

	/**
	 * Initialize a preallocated bitvector
	 * @param bv: pointer to the preallocated bitvector structure. No member should be assigned
	 * @param record_count: maximum number of records for the bitvector to hold
	 */
	void bitvector_init(bitvector_t *bv, uint record_count, uint16_t max_value);
	/**
	 * Free a preallocated bitvector
	 * @param bv: pointer to the preallocated bitvector structure
	 */
	void bitvector_kill(bitvector_t *bv);

	/**
	 * Set a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param n: data to write in the bitvector
	 * @param pos: record's index
	 */
	void bitvector_set(bitvector_t *bv, uint16_t n, uint pos);

	/**
	 * Increment a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param n: value to increment by the record
	 * @param pos: record's index
	 */
	void bitvector_inc(bitvector_t *bv, uint16_t n, uint pos);

	/**
	 * Decrement a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param n: value to decrement by the record
	 * @param pos: record's index
	 */
	void bitvector_dec(bitvector_t *bv, uint16_t n, uint pos);

	/**
	 * Get a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param pos: record's index
	 */
	uint16_t bitvector_get(bitvector_t *bv, uint pos);

	/**
	 * Set a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param v: preallocated array of int records to write in the bitvector
	 * @param edx: entry's index
	 */
	void bitvector_set_entry(bitvector_t *bv, uint16_t *v, uint edx);

	/**
	 * Get a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param v: pointer to a preallocated buffer to hold the unpacked int values
	 * @param edx: entry's index
	 */
	void bitvector_get_entry(bitvector_t *bv, uint16_t *v, uint edx);

	/**
	 * Set a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param v: preallocated array of int records to write in the bitvector
	 * @param edx: record's index
	 */
	void bitvector_set_entry_of_record(bitvector_t *bv, uint16_t *v, uint pos);

	/**
	 * Get a value in the bitvector.
	 * @param bv: pointer to the bitvector structure. Must be initialized
	 * @param v: pointer to a preallocated buffer to hold the unpacked int values
	 * @param pos: record's index
	 */
	void bitvector_get_entry_of_record(bitvector_t *bv, uint16_t *v, uint pos);

#ifdef	__cplusplus
}
#endif

#endif	/* BITVECTOR_H */


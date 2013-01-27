/**
 * @file YLZdecompressor.h
 *
 * @author Nilo Redini
 * @author Francesco Disperati
 * @author Davide Pellegrino
 *
 * Variables and functions definitions for YALZI decompression
 *
*/

#ifndef _YLZDECOMPRESSOR_H_
#define _YLZDECOMPRESSOR_H_

#include "YLZoptions.h"
#include "YLZhashtable.h"
#include "bitio.h"

/** Decompressor table entry definition */
typedef struct dec_table_entry dec_table_entry;
/** Decompressor table definition */
typedef struct dec_table_entry dec_table;

/** Decompressor hash entry*/
struct dec_table_entry{
  /** Father label */
  uint32_t f_label;
  /** index length */
  uint32_t length;
  /** Word associated */
  uint8_t* word;
  /** Word length */
  uint32_t word_length;
};

/**
   Creates a decompressor table.

   @return the table, or NULL if an error occurred
*/
dec_table* dec_table_create();

/**
   Decompress an LZ78 file.

   @param in_file Input file
   @param out_file Output file
   @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int decompress(BITIO* in_file, BITIO* out_file);

/**
   Destroys a decompressor table.

   @param dt Table to destroy
*/
void dec_table_destroy(dec_table* dt);

#endif

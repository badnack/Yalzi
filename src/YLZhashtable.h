/**
 * @file hashtable.h
 *
 * @author Nilo Redini
 * @author Francesco Disperati
 * @author Davide Pellegrino
 * Variables and functions definitions for LZ78 hash table
 *
*/

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bitio.h"

/** Hash table size */
#define HASHTABLE_SIZE 8388608 /* 24 bit */
/** Maximum amount of nodes allowed */
#define MAXNODES 4194303 /* 22 bit */
/** Bit user for the father index */
#define MAXK1VALUE ((uint32_t)1 << 14) - 1
/** Bit user for the father index */
#define MAXK2VALUE ((uint32_t)1 << 8) - 1
/** Root nSode code */
#define ROOT 0
/** First available child label */
#define FIRSTAVCHILD 257
/** Length of the fist index mask */
#define FIRSTINDEXLEN 9

/** hashtable entry definition */
typedef struct hashtable_entry hashtable_entry;
/** hashtable definition */
typedef struct hashtable_entry hashtable;

/** LZ78 Hash table entry definition*/
struct hashtable_entry{
  /* Key */
  /** father label */
  env_var f_label;
  /** child value */
  env_var c_value; //Only 8 bits are used
  /* Value */
  /** child label */
  env_var c_label;
};

/**
   Initializes a LZ78 hash table.

   @return a pointer to the LZ78 hashtable. On  error, a pointer to NULL is returned.
*/
hashtable* hashtable_create(void);

/**
   Insert an entry in a LZ78 hash table.

     @param ht LZ78 hash table.
     @param f_label Father label
     @param c_label Child label
     @param c_value Child character value
     @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int hashtable_insert(hashtable* ht, env_var f_label, env_var c_label, env_var c_value);

/**
   Deletes the whole LZ78 hash table.

   @param ht LZ78 hash table.
*/
void hashtable_destroy(hashtable* ht);

/**
   Reset the whole LZ78 hash table.

   @param ht LZ78 hash table.
   @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int hashtable_reset(hashtable* ht);

/**
   Retries the entry index of a hash table, according the values given.
   
   @param f_label Father label
   @param c_value Child character value
   @return the index value
*/
env_var hashtable_get_index(hashtable* ht, env_var f_label, env_var c_value);

#endif

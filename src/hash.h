#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>
#include <stddef.h>

#define HASHTABLE_SIZE 1024
#define hashrow        struct hashnode*
#define hashtable      struct hashnode**

struct hashnode {
  uint32_t parent;
  uint8_t symbol;
  uint32_t child;
};

hashtable hash_init();

void hash_insert( hashtable table, uint32_t parent, uint8_t symbol, uint32_t child );

void hash_destroy( hashtable table );

uint32_t hash_lookup( uint32_t parent, uint8_t symbol );

#endif /* _HASH_H_ */

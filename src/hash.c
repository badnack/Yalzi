#include "hash.h"

hashtable hash_init() {
  return calloc( HASHTABLE_SIZE, sizeof(hashrow) );
}

void hash_insert( hashtable table, uint32_t parent, uint8_t symbol, uint32_t child ) {
  uint32_t index = hash_lookup( parent, symbol );
  table[ index ] = calloc( 1, sizeof(struct hashnode) );
  table[ index ]->parent = parent;
  table[ index ]->symbol = symbol;
  table[ index ]->child  = child;
}

void hash_destroy( hashtable table ) {
  if ( table != NULL ) {
    free( table );
  }
}

uint32_t hash_lookup( uint32_t parent, uint8_t symbol ) {
  /* FIXME: stupid hash function! */
  return ( (parent ^ 0x000000FF) *
           ((parent ^ 0x0000FF00) >> 8) *
           ((parent ^ 0x00FF0000) >> 16) *
           ((parent ^ 0xFF000000) >> 24) ^
           (symbol ^ 0xFF) ) % HASHTABLE_SIZE;
}

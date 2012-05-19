#include "hash.h"

#include "cspec.h"
#include "cspec_output_verbose.h"


DESCRIBE( hash_init, " hash_init()" )

  IT( "should allocate a new hashtable" )
    hashtable table = hash_init();

    SHOULD_NOT_BE_NULL( table )
  END_IT

  IT( "should have all rows set to zero" )
    int i, rows_counter = 0;
    hashtable table = hash_init();

    for ( i = 0; i < HASHTABLE_SIZE; ++i ) {
      if ( table[i] == NULL ) rows_counter++;
    }
    SHOULD_EQUAL( rows_counter, HASHTABLE_SIZE )

  END_IT

END_DESCRIBE


DESCRIBE( hash_lookup, " hash_lookup( parent, symbol )" )

  IT( "should return a index between 0 and (HASHTABLE_SIZE-1)" )
    uint32_t parent = 0, out_of_range_counter = 0, index;
    uint8_t symbol = 0;

    for ( parent = 0; parent < HASHTABLE_SIZE; parent++ ) {
      index = hash_lookup(parent, symbol);
      if ( (index < 0) || (index >= HASHTABLE_SIZE) ) out_of_range_counter++;
    }
    SHOULD_BE_NULL( out_of_range_counter );
  END_IT

  IT( "should try to avoid collisions" )
    uint32_t parent, index, i, *collision_counter;
    uint8_t symbol;
    collision_counter = calloc( HASHTABLE_SIZE, sizeof(uint32_t) );

    for ( parent = 0; parent < HASHTABLE_SIZE; parent++ ) {
      printf("parent: %u\n", parent);
      for ( symbol = 0; symbol <= 254; ++symbol ) {
        index = hash_lookup( parent, symbol );
        collision_counter[ index ]++;
      }
    }
    for ( i = 0; i < HASHTABLE_SIZE; ++i ) {
      printf("collision_counter[%u]: %u\n", i, collision_counter[i]);
    }
  END_IT

END_DESCRIBE



void hash_spec() {
  CSpec_Run( DESCRIPTION( hash_init ),  CSpec_NewOutputVerbose() );
  CSpec_Run( DESCRIPTION( hash_lookup ), CSpec_NewOutputVerbose() );
}

#include "YLZhashtable.h"
#include <time.h>

#include "cspec.h"
#include "cspec_output_verbose.h"


DESCRIBE( get_hash, " get_hash( f_label, c_value )" )

  IT( "should always return a index between 0 and (HASHTABLE_SIZE-1)" )
    env_var f_label, c_value, out_of_range_counter = 0, index = 0;
    for ( f_label = 0; f_label <= MAXNODES; f_label++ ) {
      c_value = rand() % 256;
      index = get_hash(f_label, c_value);
      if ( (index < 0) || (index >= HASHTABLE_SIZE) ) out_of_range_counter++;
    }

    SHOULD_BE_NULL( out_of_range_counter );
  END_IT

  IT( "should try to avoid collisions" )
    env_var f_label, c_value, index, i;
    unsigned int *collision_counter, min_collisions = MAXNODES, max_collisions = 0;
    double average_collisions = 0.0;
    collision_counter = calloc( HASHTABLE_SIZE, sizeof(unsigned int) );
    srand (time(NULL));
    /* assign values */
    for ( f_label = 0; f_label <= MAXNODES; f_label++ ) {
      c_value = rand() % 256;
      index = get_hash( f_label, c_value );
      collision_counter[ index ]++;
    }
    /* collect stats */
    for ( i = 0; i < HASHTABLE_SIZE; ++i ) {
      if (collision_counter[i] < min_collisions) min_collisions = collision_counter[i];
      if (collision_counter[i] > max_collisions) max_collisions = collision_counter[i];
      if (collision_counter[i] >= 2) average_collisions += collision_counter[i];
    }
    average_collisions = average_collisions / HASHTABLE_SIZE;
    printf("Max collisions: %u\n", max_collisions);
    printf("Min collisions: %u\n", min_collisions);
    printf("Average collisions: %f\n", average_collisions);

    SHOULD_EQUAL_DOUBLE( max_collisions, 0, 10 ) /* max collisions allowed */
    SHOULD_EQUAL_DOUBLE( average_collisions, 0, 1.0 ) /* max avg collisions allowed */
  END_IT

END_DESCRIBE


DESCRIBE( hashtable_create, " hashtable_create()" )

  IT( "should allocate a new hashtable" )
    hashtable* table = hashtable_create();

    SHOULD_NOT_BE_NULL( table )
  END_IT

  IT( "should have first children set" )
    int i, index, counter = 0;
    hashtable* table = hashtable_create();
    for ( i = 0; i < FIRSTAVCHILD-1; ++i ) {
      if (table[get_hash(ROOT, i)].c_value == i) counter++;
    }
    SHOULD_EQUAL( counter, FIRSTAVCHILD-1 )
  END_IT

END_DESCRIBE



void YLZhashtable_spec() {
  CSpec_Run( DESCRIPTION( get_hash ), CSpec_NewOutputVerbose() );
  CSpec_Run( DESCRIPTION( hashtable_create ),  CSpec_NewOutputVerbose() );
}

#include "bitio.h"
#include "cspec.h"

DESCRIBE( bit_open, " bit_open( char* filepath, int mode )" )

  IT( "should return a valid bitfile pointer when opening a nonexistent file in WRITE mode" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );

    SHOULD_NOT_BE_NULL( bitfile )
    SHOULD_NOT_BE_NULL( bitfile->fd )
    SHOULD_EQUAL( bitfile->mode, mode )
  END_IT


  IT( "should return null when opening a nonexistent file in READ mode" )
    int mode = 0;
    struct bitfile* bitfile = bit_open( "foobar", mode );

    SHOULD_BE_NULL( bitfile )
  END_IT


  IT( "should return null if mode is invalid" )
    int mode = 123;
    struct bitfile* bitfile = bit_open( "foo", mode );

    SHOULD_BE_NULL( bitfile )
  END_IT

END_DESCRIBE

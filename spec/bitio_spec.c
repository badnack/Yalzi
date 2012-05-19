#include "bitio.h"

#include "cspec.h"
#include "cspec_output_verbose.h"

/* Spec support functions */
char read_char( const char* filename, int position, int delete ) {
  FILE* fp = fopen( filename, "r" );
  fseek( fp, position, SEEK_SET );
  char c = fgetc(fp);
  fclose( fp );
  if ( delete ) remove( filename );
  return c;
}

int file_size( const char* filename, int delete ) {
  FILE* fp = fopen( filename, "r" );
  fseek( fp, 0L, SEEK_END );
  int size = ftell(fp);
  fclose( fp );
  if ( delete ) remove( filename );
  return size;
}


/* Spec */
DESCRIBE( bit_open, " bit_open( char* name, int mode )" )

  IT( "should return a valid bitfile pointer when opening a nonexistent file in WRITE mode" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );

    SHOULD_NOT_BE_NULL( bitfile )
    SHOULD_NOT_BE_NULL( bitfile->fd )
    SHOULD_EQUAL( bitfile->mode, mode )

    remove("foo");
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

    remove("foo");
  END_IT

END_DESCRIBE


DESCRIBE( bit_close, " bit_close( struct bitfile* p )" )

  IT( "should return 0" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );

    SHOULD_BE_NULL( bit_close(bitfile) )

    remove("foo");
  END_IT

  IT( "should return -1 when invoked multiple times" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );

    SHOULD_PENDING( "fixme" )
    /* SHOULD_BE_NULL( bit_close(bitfile) ) */
    /* SHOULD_EQUAL( bit_close(bitfile), -1 ) */

    remove("foo");
  END_IT

END_DESCRIBE


DESCRIBE( bit_write, " bit_write( struct bitfile* p, uint64_t data, int len )" )

  IT( "should not write to file until bit_close()" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );
    bit_write( bitfile, 0xFF, 8 );

    SHOULD_EQUAL( read_char("foo",0,1), EOF )

    bit_close( bitfile );
  END_IT

  IT( "should write to file when reaching buffer size" )
    int i, mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );
    for ( i = 0; i < NMAX; ++i ) bit_write( bitfile, 0xAA, 8 );

    SHOULD_EQUAL( file_size("foo",1), 512 )
  END_IT

  IT( "should write at least 1 bit to file" )
    int mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );
    bit_write( bitfile, 0xFF, 1 );
    bit_close( bitfile );

    SHOULD_EQUAL( read_char("foo",0,1), 0x01 )
  END_IT

  IT( "should write byte in little-endian mode" )
    int i, mode = 1;
    struct bitfile* bitfile = bit_open( "foo", mode );
    bit_write( bitfile, 0xAAABACADAEAFB0B1, 64 );
    bit_close( bitfile );

    for ( i = 0; i < 8; ++i ) {
      int delete = (i==7)? 1 : 0;
      SHOULD_EQUAL( read_char("foo",i,delete), (char) 0xB1-i )
    }
  END_IT

END_DESCRIBE


DESCRIBE( bit_read, " bit_read( ... )" )

  IT( "should read an arbitrary length of bit" )
    SHOULD_PENDING( "IMPLEMENT ME!" )
  END_IT

END_DESCRIBE



void bitio_spec() {
  CSpec_Run( DESCRIPTION( bit_open ),  CSpec_NewOutputVerbose() );
  CSpec_Run( DESCRIPTION( bit_close ), CSpec_NewOutputVerbose() );
  CSpec_Run( DESCRIPTION( bit_write ), CSpec_NewOutputVerbose() );
  CSpec_Run( DESCRIPTION( bit_read ),  CSpec_NewOutputVerbose() );
}

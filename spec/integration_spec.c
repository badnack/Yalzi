#include <stdio.h>

#include "cspec.h"
#include "cspec_output_verbose.h"

#define BIN "./bin/yalzi"
#define PIC_FILE "./spec/fixtures/stormtrooperlove.jpg"


DESCRIBE( compress_and_extract, " compress and extract a file" )

  IT( "should compress a binary file, and then extract it" )
    int ok_compression = system(BIN " -c " PIC_FILE);
    int ok_extraction =  system(BIN " -x " PIC_FILE".ylz " PIC_FILE".out");
    SHOULD_EQUAL( ok_compression, 0 )
    SHOULD_EQUAL( ok_extraction, 0 )
    SHOULD_EQUAL( file_size(PIC_FILE, 0), file_size(PIC_FILE".out", 0) )

    FILE* orig = fopen( PIC_FILE, "r" );
    FILE* test = fopen( PIC_FILE".out", "r" );
    int c, equal_bytes = -1;
    for ( c = 0; c != EOF; ) {
      c = fgetc(orig);
      if ( c == fgetc(test) ) equal_bytes++;
    }
    SHOULD_EQUAL( equal_bytes, file_size(PIC_FILE, 0) )
    fclose(orig);
    fclose(test);
  END_IT

END_DESCRIBE



void integration_spec() {
  CSpec_Run( DESCRIPTION( compress_and_extract ),  CSpec_NewOutputVerbose() );
}

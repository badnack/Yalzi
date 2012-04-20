#include "cspec.h"
#include "cspec_output_verbose.h"


DEFINE_DESCRIPTION( bit_open )

int main() {
  CSpec_Run( DESCRIPTION( bit_open ), CSpec_NewOutputVerbose() );

  return 0;
}

#include <stdio.h>


/* Spec helper functions */
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

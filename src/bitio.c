#include "bitio.h"

struct bitfile* bit_open( char* name, int mode ) {
  int fd;
  struct bitfile* p;

  if ( mode != 0 && mode != 1 ) {
    errno = EINVAL;
    return NULL;
  }

  fd = open( name, (mode == 0)? O_RDONLY : O_CREAT|O_WRONLY, 0644 );
  if ( fd < 0 ) return NULL;

  p = calloc( 1, sizeof(struct bitfile) );

  if ( p == NULL ) {
    close( fd );
    errno = ENOMEM;
    return NULL;
  }

  p->fd = fd;
  p->mode = mode;

  return p;
}


int bit_close( struct bitfile* p ) {
  int offs = p->pos / 8;
  if ( p->pos % 8 != 0 )  offs++;

  write( p->fd, p->buff, offs );
  close( p->fd );
  bzero( p, sizeof( struct bitfile ) );
  free( p );
  return 0;
}


int bit_write( struct bitfile* p, uint64_t data, int len ) {
  int i, offs, k;
  unsigned char word;

  for ( i = 0; i < len; i++ ) {
    offs = p->pos / 8;
    k    = p->pos % 8;
    word = ( data & ( 1LL << i )) ? 1 : 0;

    if ( word == 0 ) {
      p->buff[offs] = p->buff[offs] & ~( 1<<k );
    }
    else {
      p->buff[offs] = p->buff[offs] | ( 1<<k );
    }

    p->pos++;

    if ( p->pos == NMAX*8 ){
      write( p->fd, p->buff, NMAX );
      p->pos = 0;
    }
  }

  return 0;
}

#ifndef _BITIO_H_
#define _BITIO_H_

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NMAX 512
#define LMAX 128

struct bitfile {
  int fd;
  char buff [NMAX];
  int pos;
  int len;
  int mode;
};

struct bitfile* bit_open( char*, int );

int bit_read( struct bitfile*, uint64_t, int );

int bit_write( struct bitfile*, uint64_t, int );

int bit_close( struct bitfile* );

#endif /* _BITIO_H_ */

#include <stdio.h>
#include <fcntl.h>
#include "YLZhashtable.h"
#include "bitio.h"
#include "YLZcompressor.h"
#include "YLZdecompressor.h"

int
main(){
  BITIO* in_file, *out_file;

  // compression
  printf ("Compression\n");
  if((in_file = bitio_open("/tmp/asd.jpg", O_RDONLY)) == NULL)
    return -1;
  if((out_file = bitio_open("/tmp/asd.jpg.yz", O_WRONLY)) == NULL)
    return -1;

  compress(in_file, out_file);
  bitio_close(in_file);
  bitio_close(out_file);

  printf ("Decompression\n");

  // decompression
  if((in_file = bitio_open("/tmp/asd.jpg.yz", O_RDONLY)) == NULL)
    return -1;
  if((out_file = bitio_open("/tmp/asd_dec.jpg", O_WRONLY)) == NULL)
    return -1;

  decompress(in_file, out_file);
  bitio_close(in_file);
  bitio_close(out_file);


  return 0;
}

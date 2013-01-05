#include <stdio.h>
#include <fcntl.h>
#include "LZ78hashtable.h"
#include "bitio.h"
#include "LZ78compressor.h"

int
main(){
  BITIO* in_file, *out_file;
  
  // compression
  if((in_file = bitio_open("/tmp/asd.jpg", O_RDONLY)) == NULL)
    return -1;
  if((out_file = bitio_open("/tmp/asd.jpg.yz", O_WRONLY)) == NULL)
    return -1;

  lz78_compress(in_file, out_file);
  bitio_close(in_file);
  bitio_close(out_file);

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

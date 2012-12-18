#include <stdio.h>
#include <fcntl.h>
#include "LZ78hashtable.h"
#include "bitio.h"

int
main(){
  BITIO* file;
  hashtable* ht;
  uint64_t read[2];

  /* bitio test */
  file = bitio_open("/home/badnack/Desktop/asd", O_WRONLY);

  read[0] = 1;
  read[1] = 2;

  bitio_write(file, read, 128);
  bitio_close(file);

  file = bitio_open("/home/badnack/Desktop/asd", O_RDONLY);
  bitio_read(file, read, 128);
  bitio_close(file);
  printf("\n%llu-%llu\n", read[0], read[1]);

  /* hash table test */
  ht = hashtable_create();
  hashtable_insert(ht, 1, 258, 'c');
  hashtable_print(ht);
  hashtable_destroy(ht);

  return 0;
}

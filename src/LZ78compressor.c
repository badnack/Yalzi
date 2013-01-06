#include "LZ78compressor.h"
#include "LZ78hashtable.h"
#include <fcntl.h>

int
lz78_compress(BITIO* in_file, BITIO* out_file)
{
  hashtable* ht;
  int byte_read, i;
  uint8_t byte_buff[BYTEBUFFERSIZE];
  uint32_t c_label, f_label, index_mask;
  uint32_t c_label_count;
  size_t index_length;
  env_var root = ROOT;//FIXME
  FILE* in_buffered_file;

  if(in_file == NULL || out_file == NULL)
    return -1;
  if((ht = hashtable_create()) == NULL)
    return -1;

  if((in_buffered_file = fdopen(in_file->fd, "r")) == NULL)
    return -1;

  c_label_count = FIRSTAVCHILD;
  f_label = ROOT;
  index_length = FIRSTINDEXLEN;
  index_mask = (1 << index_length) - 1;

  memset(byte_buff, 0, BYTEBUFFERSIZE * sizeof(uint8_t));

  while(!feof(in_buffered_file) && !ferror(in_buffered_file)){
    byte_read = fread(byte_buff, 1, BYTEBUFFERSIZE * sizeof(uint8_t), in_buffered_file);

    for(i = 0; i < byte_read; i++){

      if((c_label = hashtable_get_index(ht, f_label, byte_buff[i])) == ROOT){
        bitio_write(out_file, &f_label, index_length); //FIXME add checks
        hashtable_insert(ht, f_label, c_label_count, byte_buff[i]); //also here
        f_label = byte_buff[i] + 1; // next position to start if first 256 are consecutive

        if(!(++c_label_count & index_mask)){
          index_length++;
          index_mask = (index_mask << 1) | 1;
        }

        if(c_label_count == MAXNODES){
          hashtable_reset(ht); //FIXME add checks
          c_label_count = FIRSTAVCHILD;
          index_length = FIRSTINDEXLEN;
          index_mask = (1 << index_length) - 1;
          f_label = ROOT;
        }
      }
      else
        f_label = c_label;
    }

  }

  bitio_write(out_file, &f_label, index_length); //FIXME add checks
  bitio_write(out_file, &root, index_length);
  hashtable_destroy(ht);
  return 0;
}

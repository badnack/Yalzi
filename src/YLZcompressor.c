#include <fcntl.h>
#include <errno.h>
#include "YLZcompressor.h"
#include "YLZhashtable.h"

int
compress(BITIO* in_file, BITIO* out_file)
{
  hashtable* ht;
  int byte_read, i, err_val;
  uint8_t byte_buff[BYTEBUFFERSIZE];
  uint32_t c_label, f_label, index_mask;
  uint32_t c_label_count;
  size_t index_length;
  env_var root = ROOT;
  FILE* in_buffered_file;

  if(in_file == NULL || out_file == NULL){
    errno = EINVAL;
    return -1;
  }
  if((ht = hashtable_create()) == NULL)
    return -1;
  if((in_buffered_file = fdopen(in_file->fd, "r")) == NULL)
    return -1;

  c_label_count = FIRSTAVCHILD;
  f_label = ROOT;
  index_length = FIRSTINDEXLEN;
  index_mask = (1 << index_length) - 1;
  err_val = 0;
  memset(byte_buff, 0, BYTEBUFFERSIZE * sizeof(uint8_t));

  while(!feof(in_buffered_file) && !ferror(in_buffered_file) && !err_val){

    byte_read = fread(byte_buff, 1, BYTEBUFFERSIZE * sizeof(uint8_t), in_buffered_file);

    for(i = 0; i < byte_read; i++){

      if((c_label = hashtable_get_index(ht, f_label, byte_buff[i])) == ROOT){
        if(bitio_write(out_file, &f_label, index_length) == -1 ||
           hashtable_insert(ht, f_label, c_label_count, byte_buff[i]) == -1){
          err_val = -1;
          break;
        }
        f_label = byte_buff[i] + 1;

        if(!(++c_label_count & index_mask)){
          index_length++;
          index_mask = (index_mask << 1) | 1;
        }

        if(c_label_count == MAXNODES){
          if(hashtable_reset(ht) == -1){
            err_val = -1;
            break;
          }
          c_label_count = FIRSTAVCHILD;
          index_length = FIRSTINDEXLEN;
          index_mask = (1 << index_length) - 1;
        }
      }
      else
        f_label = c_label;
    }

  }

  if(ferror(in_buffered_file)){
    errno = EBADFD;
    err_val = -1;
  } else {
    if(bitio_write(out_file, &f_label, index_length) == -1 ||
       bitio_write(out_file, &root, index_length) == -1)
      err_val = -1;
  }

  bitio_flush(out_file);
  hashtable_destroy(ht);

  return err_val;
}

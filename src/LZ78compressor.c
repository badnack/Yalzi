#include "LZ78compressor.h"
#include "LZ78hashtable.h"
#include <fcntl.h>


int
lz78_compress(BITIO* in_file, BITIO* out_file)
{
  hashtable* ht;
  int bit_read, i;
  env_var env_buff[BYTEBUFFERSIZE];
  uint8_t* byte_buff;
  uint32_t c_label, f_label, index_mask;
  uint32_t c_label_count;
  size_t index_length;
  env_var root = ROOT;//FIXME
  /* BITIO* test; */

  if(in_file == NULL || out_file == NULL)
    return -1;
  if((ht = hashtable_create()) == NULL)
    return -1;

  f_label = root;
  c_label_count = 257;
  index_length = 9;
  index_mask = (1 << index_length) - 1;
  /* if((test = bitio_open("/home/badnack/lol.jpg", O_WRONLY)) == NULL) */
  /*   printf("NULL"); */
  memset(env_buff, 0, BYTEBUFFERSIZE * sizeof(env_var));

  while(1){ //FIXME change with a function which manages EOF
    if((bit_read = bitio_read(in_file, env_buff, BYTEBUFFERSIZE * sizeof(env_var) * 8)) <= 0)
      break;
    byte_buff = (uint8_t*)env_buff;
    /* bitio_write(test, byte_buff, bit_read); */

    for(i = 0; i < bit_read / 8; i++){

      if((c_label = hashtable_get_index(ht, f_label, byte_buff[i])) == ROOT){
        bitio_write(out_file, &f_label, index_length); //FIXME add checks
        hashtable_insert(ht, f_label, byte_buff[i], c_label_count); //also here
        /* f_label = byte_buff[i] + 1; // next position to start */
        f_label = hashtable_get_index(ht, ROOT, byte_buff[i]);

        if(!(++c_label_count & index_mask)){
          index_length++;
          index_mask = (index_mask << 1) | 1;
        }

        if(c_label_count == MAXNODES){
          hashtable_reset(ht); //FIXME add checks
          c_label_count = 257;
          index_length = 9;
          index_mask = (1 << index_length) - 1;
          f_label = ROOT;
        }
      }
      else
        f_label = c_label;
    }

  }
  /* bitio_close(test); */
  //last write
  bitio_write(out_file, &f_label, index_length); //FIXME add checks
  bitio_write(out_file, &root, index_length);
  hashtable_destroy(ht);
  return 0;
}

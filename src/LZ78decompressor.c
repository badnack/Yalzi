#include "LZ78decompressor.h"
#include <errno.h>

dec_table*
dec_table_create()
{
  dec_table* dt;
  int i;

  if((dt = (dec_table*) calloc(MAXNODES, sizeof(dec_table))) == NULL)
    return NULL;

  for(i = 0; i < FIRSTAVCHILD; i++){
    dt[i].length = 1;
    dt[i].f_label = ROOT;
    
    if((dt[i].word = malloc(1 * sizeof(uint8_t))) == NULL){
      dec_table_destroy(dt);
      free(dt);
      return NULL;
    }
    dt[i].word[0] = (uint8_t)i;
  }

  return dt;
}

void
dec_table_destroy(dec_table* dt)
{
  int n = MAXNODES;

  if(dt == NULL)
    return;
  for(; n--;)
    if(dt[n].word != 0)
      free(dt[n].word);

  memset(dt, 0, sizeof(dec_table) * MAXNODES);
}

int 
decompress(BITIO* in_file, BITIO* out_file)
{
  int err_val;
  uint32_t current_index, index_mask;
  size_t index_length;
  uint32_t c_label;
  dec_table* dt;
  dec_table_entry* dte;

  if(in_file == NULL || out_file == NULL){
    errno = EINVAL;
    return -1;
  }

  if((dt = dec_table_create()) == NULL)
    return -1;

  index_length = FIRSTINDEXLEN;
  index_mask = (1 << index_length) - 1;
  c_label = FIRSTAVCHILD;
  err_val = 0;

  while(1){
    current_index = 0;

    if(bitio_read(in_file, &current_index, index_length) < index_length || current_index > c_label){
      err_val = -1;
      break;
    }

    if(current_index == ROOT)
      break;
    
    dte = &dt[current_index];

    if(c_label > FIRSTAVCHILD){
      //....          
    }

    if(bitio_write(out_file, dte->word, dte->length * 8) == -1){
      err_val = -1;
      break;
    }

    dte = &dt[c_label];
    dte->f_label = current_index;
    c_label++;

    if(!(c_label & index_length)){
      index_length++;
      index_mask = (index_length << 1) | 1;
    }
    if(c_label == MAXNODES){
      c_label = FIRSTAVCHILD;
      index_length = FIRSTINDEXLEN;
      index_mask = (1 << index_length) - 1;
    }
  }

  dec_table_destroy(dt);

  return err_val;
}

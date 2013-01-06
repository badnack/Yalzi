#include "LZ78decompressor.h"
#include <errno.h>

static void
pre_append(dec_table* current, dec_table* parent)
{
    if(current->buffer_length < (current->length + parent->length)){
      current->buffer_length = current->length + parent->length;
      current->word = realloc(current->word, current->buffer_length);
    }
    if(current->length > 0){
      memmove(current->word + parent->length, current->word, current->length);
    }
    memcpy(current->word, parent->word, parent->length);
    current->length += parent->length;
}


dec_table*
dec_table_create()
{
  dec_table* dt;
  int i;

  if((dt = (dec_table*) calloc(MAXNODES, sizeof(dec_table))) == NULL)
    return NULL;

  for(i = 1; i < FIRSTAVCHILD; i++){
    dt[i].length = 1;
    dt[i].f_label = ROOT;

    if((dt[i].word = malloc(1 * sizeof(uint8_t))) == NULL){
      dec_table_destroy(dt);
      free(dt);
      return NULL;
    }
    dt[i].word[0] = (uint8_t)i - 1;
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
  dec_table_entry* current;
  dec_table_entry* last;
  dec_table_entry* dte;
  FILE* out_buffered_file;

  if(in_file == NULL || out_file == NULL){
    errno = EINVAL;
    return -1;
  }

  if((out_buffered_file = fdopen(out_file->fd, "w")) == NULL){
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

    current = &dt[current_index];

    if(c_label > FIRSTAVCHILD){
      last = &dt[c_label - 1];
      if(last->length > 0)
        last->length = 0;

      if(current_index == (c_label - 1)){
        dte = &dt[last->f_label];
        while(dte->length == 1 && dte->f_label != ROOT){
          pre_append(last, dte);
          dte = &dt[dte->f_label];
        }
        pre_append(last, dte);
        last->length++;
        if(last->buffer_length < last->length){
          last->buffer_length = last->length;
          last->word = realloc(last->word, last->buffer_length);
        }
        last->word[last->length - 1] =  last->word[0];
      }
      else{
        if(last->buffer_length == 0){
          last->buffer_length = 1;
          last->word = realloc(last->word, 1);
        }
        last->length = 1;
        if(last->word == NULL){
          err_val = -1;
          break;
        }

        if(current_index > (FIRSTAVCHILD - 1) && current->length == 1){
          dte = &dt[current->f_label];
          while(dte->length == 1 && dte->f_label != ROOT){
            pre_append(current, dte);
            dte = &dt[dte->f_label];
          }
          pre_append(current, dte);
          last->word[0] = dte->word[0];
        }
        else
          last->word[0] = current->word[0];
      }
    }

    if(fwrite(current->word, 1, current->length, out_buffered_file) != current->length){
      err_val = -1;
      break;
    }

    current = &dt[c_label];
    current->f_label = current_index;
    if(!(++c_label & index_mask)){
      index_length++;
      index_mask = (index_mask << 1) | 1;
    }

    if(c_label == MAXNODES){
      c_label = FIRSTAVCHILD;
      index_length = FIRSTINDEXLEN;
      index_mask = (1 << index_length) - 1;
    }
  }

  dec_table_destroy(dt);
  fflush(out_buffered_file);

  return err_val;
}

#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "YLZoptions.h"
#include "YLZcompressor.h"
#include "YLZhashtable.h"

int
compress(BITIO* in_file, BITIO* out_file)
{
  hashtable* ht;
  int byte_read, i, err_val;
  off_t file_length = 0, file_read = 0;
  uint8_t byte_buff[BYTEBUFFERSIZE];
  env_var c_label, f_label, index_mask;
  env_var c_label_count;
  size_t index_length;
  env_var root = ROOT;
  FILE* in_buffered_file = NULL;
  struct stat file_stat;
  int progress = 0, percentage = 0;

  if(in_file == NULL || out_file == NULL){
    errno = EINVAL;
    return -1;
  }
  if(verbose_flag)
    if ((write(STDOUT_FILENO, "Inizializing Hash Table\n",  25))){/*Shut up compiler*/}
  if((ht = hashtable_create()) == NULL)
    return -1;
  if((in_buffered_file = fdopen(in_file->fd, "r")) == NULL)
    return -1;
  if(fstat(in_file->fd,&file_stat) < 0)    
    return 1;
  
  file_length = file_stat.st_size;
  c_label_count = FIRSTAVCHILD;
  f_label = ROOT;
  index_length = FIRSTINDEXLEN;
  index_mask = (1 << index_length) - 1; //Save the number of bits used to represent the current hash length
  err_val = 0;
  memset(byte_buff, 0, BYTEBUFFERSIZE * sizeof(uint8_t));

  if (verbose_flag)
    if ((write(STDOUT_FILENO, "Start compressing ... \nPercentage of the Compressed File\n0        50       100\n",  80))){/*Shut up compiler*/}
    
  while(!feof(in_buffered_file) && !ferror(in_buffered_file) && !err_val){

    byte_read = fread(byte_buff, 1, BYTEBUFFERSIZE * sizeof(uint8_t), in_buffered_file);
    file_read += byte_read;

    if (verbose_flag){
      percentage = ((file_read*100)/file_length);
      while( percentage > progress){
        progress = progress + 5;
        if ((write(STDOUT_FILENO, "-",  2))){/*Shut up compiler*/}
      }
    }

    for(i = 0; i < byte_read; i++){
      if((c_label = hashtable_get_index(ht, f_label, byte_buff[i])) == ROOT){ //write the encoding of the max length sequence of char in out_file (when the next hash index is ROOT) 
        if(bitio_write(out_file, &f_label, index_length) == -1 ||
           hashtable_insert(ht, f_label, c_label_count, byte_buff[i]) == -1){
          err_val = -1;
          break;
        }
        f_label = byte_buff[i] + 1;

        if(!(++c_label_count & index_mask)){ //check if the number of bits to represent the child is not greater than current size,otherwise increment index_mask  
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
    errno = EBADF;
    err_val = -1;
  } else {
    if(bitio_write(out_file, &f_label, index_length) == -1 || //Save last encoding
       bitio_write(out_file, &root, index_length) == -1)
      err_val = -1;
  }
  
  if(!err_val)
    bitio_flush(out_file);

  hashtable_destroy(ht);
  fclose(in_buffered_file);

  if (verbose_flag)
    if ((write(STDOUT_FILENO, "\nDestroing Hash Table.\nCompression terminated.\n",  48))){/*Shut up compiler*/}

  return err_val;
}

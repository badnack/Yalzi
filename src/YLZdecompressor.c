#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "YLZoptions.h"
#include "YLZdecompressor.h"


static void
pre_append(dec_table* current, dec_table* parent)
{
    if (current->word_size < (current->word_length + parent->word_length)) {
      current->word_size = current->word_length + parent->word_length;
      current->word = realloc(current->word, current->word_size);
    }

    if (current->word_length > 0) {
      memmove(current->word + parent->word_length, current->word, current->word_length);
    }

    memcpy(current->word, parent->word, parent->word_length);
    current->word_length += parent->word_length;
}

dec_table*
dec_table_create()
{
  dec_table* dt;
  int i;

  if ((dt = (dec_table*) calloc(MAXNODES, sizeof(dec_table))) == NULL)
    return NULL;

  for (i = 1; i < FIRSTAVCHILD; i++) {
    dt[i].word_length = 1;
    dt[i].f_label = ROOT;

    if ((dt[i].word = malloc(1 * sizeof(uint8_t))) == NULL) {
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

  if (dt == NULL)
    return;

  for (; n--;)
    if(dt[n].word != 0)
      free(dt[n].word);

  memset(dt, 0, sizeof(dec_table) * MAXNODES);
  free(dt);
}

int
decompress(BITIO* in_file, BITIO* out_file)
{
  int err_val, bit_read;
  env_var current_index, index_mask;
  size_t index_length;
  uint32_t c_label;
  dec_table* dt;
  dec_table_entry* current;
  dec_table_entry* last;
  dec_table_entry* dte;
  FILE* out_buffered_file = NULL;
  off_t file_length, file_read;
  struct stat file_stat;
  int progress = 0, percentage = 0;

  if (in_file == NULL || out_file == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (in_file->mode != O_RDONLY || out_file->mode != O_WRONLY) {
    print_verbose("Error: unable to open input/output file\n");
    return -1;
  }
  if ((out_buffered_file = fdopen(out_file->fd, "w")) == NULL) {
    print_verbose("Error: unable to open output file\n");
    return -1;
  }

  print_verbose("Initializing the decompression table\n");

  if ((dt = dec_table_create()) == NULL) {
    print_verbose("Error: unable to create the decompression table\n");
    return -1;
  }
  if (fstat(in_file->fd, &file_stat) < 0) {
    print_verbose("Error: file does not exist\n");
    return 1;
  }

  file_length = file_stat.st_size;
  index_length = FIRSTINDEXLEN;
  index_mask = (1 << index_length) - 1;
  c_label = FIRSTAVCHILD;
  err_val = 0;
  file_read = 0;

  print_verbose("Start decompressing ... \nPercentage of the Decompressed File\n0        50       100\n");

  while (1) {

    current_index = 0;
    bit_read = bitio_read(in_file, &current_index, index_length);
    file_read += bit_read;

    if (verbose_flag) {
      percentage = (((file_read / 8)*100) / file_length);
      while(percentage > progress){
        progress = progress + 5;
        print_verbose("-");
      }
    }

    if (bit_read == -1 || (env_var)bit_read < index_length || current_index > c_label) {
      err_val = -1;
      break;
    }

    if (current_index == ROOT)
      break;

    current = &dt[current_index];

    /* In first round no previous child exists */
    if (c_label > FIRSTAVCHILD) {
      last = &dt[c_label - 1];
      if (last->word_length > 0)
        last->word_length = 0;

      if (current_index == (c_label - 1)) {
        dte = &dt[last->f_label];
        while (dte->word_length == 1 && dte->f_label != ROOT) {
          pre_append(last, dte);
          dte = &dt[dte->f_label];
        }
        pre_append(last, dte);
        last->word_length++;
        if (last->word_size < last->word_length) {
          last->word_size = last->word_length;
          last->word = realloc(last->word, last->word_size);
        }
        last->word[last->word_length - 1] =  last->word[0];
      }
      else {
        if (last->word_size == 0) {
          last->word_size = 1;
          last->word = realloc(last->word, 1);
        }
        last->word_length = 1;
        if (last->word == NULL) {
          err_val = -1;
          break;
        }

        /* current value has to be completed */
        if (current_index > (FIRSTAVCHILD - 1) && current->word_length == 1) {
          dte = &dt[current->f_label];
          while (dte->word_length == 1 && dte->f_label != ROOT) {
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

    if (fwrite(current->word, 1, current->word_length, out_buffered_file) != current->word_length) {
      err_val = -1;
      break;
    }

    current = &dt[c_label];
    current->f_label = current_index;

    if (!(++c_label & index_mask)) {
      index_length++;
      index_mask = (index_mask << 1) | 1;
    }

    if (c_label == MAXNODES) {
      c_label = FIRSTAVCHILD;
      index_length = FIRSTINDEXLEN;
      index_mask = (1 << index_length) - 1;
    }
  }

  if (!err_val)
    print_verbose("Decompression succeded!\n");
  else
    print_verbose("Error: Decompression failed!\n");

  print_verbose("Destroying the decompression table\n");
  dec_table_destroy(dt);
  fflush(out_buffered_file);
  fclose(out_buffered_file);

  return err_val;
}

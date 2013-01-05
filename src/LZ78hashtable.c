#include "LZ78hashtable.h"

/* INLINE? */
static uint32_t
get_hash(uint32_t f_label, uint8_t c_value)
{
  return (((f_label & MAXNODES) << BITSFATHER) | (c_value & 255)) % MAXNODES; //FIXME: 255 as bits in a define
}

static int
init_dictionary(hashtable* ht)
{
  int i;

  if(ht == NULL)
    return -1;

  memset(ht, 0, HASHTABLE_SIZE * sizeof(hashtable));

  for(i = 0; i < FIRSTAVCHILD - 1; i++){
    if(hashtable_insert(ht, ROOT, (uint32_t)i+1, (uint8_t)i) == -1){
      hashtable_destroy(ht);
      return -1;
    }
  }

  return 0;
}

int
hashtable_insert(hashtable* ht, uint32_t f_label, uint32_t c_label, uint8_t c_value)
{
  uint32_t index;

  if(ht == NULL || c_label > MAXNODES)
    return -1;

  index = get_hash(f_label, c_value);
  while(ht[index].c_label){
    if((index = (index + 1) % HASHTABLE_SIZE) == MAXNODES)
      return -1;
  }

  ht[index].c_label = c_label;
  ht[index].f_label = f_label;
  ht[index].c_value = c_value;

  return 0;
}

uint32_t
hashtable_get_index(hashtable* ht, uint32_t f_label, uint8_t c_value)
{
  uint32_t h_index;

  if(ht == NULL)
    return -1;

  h_index = get_hash(f_label, c_value);

  while(ht[h_index].c_label && (ht[h_index].c_value != c_value || ht[h_index].f_label != f_label))
    h_index = (h_index + 1) % MAXNODES;

  return ht[h_index].c_label;
}

/*FIXME set errno!!!*/
hashtable*
hashtable_create()
{
  hashtable* ht;

  if((ht = (hashtable_entry*)calloc(HASHTABLE_SIZE, sizeof(hashtable_entry))) == NULL )
    return NULL;
  if(init_dictionary(ht) == -1)
    return NULL;
  return ht;
}

int
hashtable_reset(hashtable* ht)
{
  return init_dictionary(ht);
}

void
hashtable_destroy(hashtable* ht)
{
  memset(ht, 0, HASHTABLE_SIZE* sizeof(hashtable));
  free(ht);
}

void
hashtable_print(hashtable* ht)
{
  int i, valid;

  if(ht == NULL)
    return;

  for(valid = i = 0; i < HASHTABLE_SIZE; i++){
    if(ht[i].c_label){
      valid++;
      printf("%i)\n Father label: %i.\n Child label: %i.\n Child value: %c.\n\n", valid, ht[i].f_label, ht[i].c_label, ht[i].c_value);
    }
  }
}

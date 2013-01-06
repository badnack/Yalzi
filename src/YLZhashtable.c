#include "YLZhashtable.h"

static inline uint32_t
get_hash(uint32_t k1, uint8_t k2)
{
  return (((k1 & MAXK1VALUE) << 8) | (k2 & MAXK2VALUE))  & (HASHTABLE_SIZE - 1);
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
  while(ht[index].c_label)
    index = (index + 1) & (HASHTABLE_SIZE - 1);

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

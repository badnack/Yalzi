#include "YLZhashtable.h"

static inline uint32_t
get_hash(env_var k1, env_var k2)
{
  return ((k1 ^ 0x000000FF) *
          ((k1 ^ 0x0000FF00) >> 8) *
          ((k1 ^ 0x00FF0000) >> 16) *
          ((k1 ^ 0xFF000000) >> 24) ^
          (k2 ^ 0xFF) ) % HASHTABLE_SIZE;
}

static int
init_dictionary(hashtable* ht)
{
  int i;

  if(ht == NULL)
    return -1;

  memset(ht, 0, HASHTABLE_SIZE * sizeof(hashtable));

  for(i = 0; i < FIRSTAVCHILD - 1; i++){
    if(hashtable_insert(ht, ROOT, (env_var)i+1, (env_var)i) == -1){
      hashtable_destroy(ht);
      return -1;
    }
  }

  return 0;
}

int
hashtable_insert(hashtable* ht, env_var f_label, env_var c_label, env_var c_value)
{
  uint32_t index;

  if(ht == NULL || c_label > MAXNODES)
    return -1;

  index = get_hash(f_label, c_value);
  while(ht[index].c_label)
    index = (index + 1) % HASHTABLE_SIZE;

  ht[index].c_label = c_label;
  ht[index].f_label = f_label;
  ht[index].c_value = c_value;

  return 0;
}

env_var
hashtable_get_index(hashtable* ht, env_var f_label, env_var c_value)
{
  uint32_t h_index;

  if(ht == NULL)
    return -1;

  h_index = get_hash(f_label, c_value);

  while(ht[h_index].c_label && (ht[h_index].c_value != c_value || ht[h_index].f_label != f_label))
    h_index = (h_index + 1) % HASHTABLE_SIZE;

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

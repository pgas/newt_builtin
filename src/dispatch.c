#include "dispatch.h" 


HASH_TABLE *dispatch_table_create(size_t n, entry_point table[]){
  HASH_TABLE * htable = hash_create(1);
  for (size_t i = 0; i < n; i+=1) {
    BUCKET_CONTENTS * bucket = hash_search(table[i].name, htable, HASH_CREATE);
    bucket->data = (PTR_T) &table[i];
  }
  return htable;
}


entry_point *dispatch_table_find(const char *name, HASH_TABLE *htable){
  BUCKET_CONTENTS * bucket = hash_search(name, htable, 0);
  if (bucket != NULL) {
    return (entry_point *) bucket->data;
  } else {
    return NULL;
  }
}

int dispatch_table_run(const char *name, WORD_LIST* args, HASH_TABLE *htable){
  entry_point * e = dispatch_table_find(name, htable);
  return e->function(args);
}

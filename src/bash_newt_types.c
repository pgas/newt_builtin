#include "bash_newt_types.h"
#include <search.h>

#include <stdio.h>

#include "bash_includes.h"

struct bash_newt_component_struct {
  newtComponent  co;
  char * destroy_callback;
  char checkbok_result;
};

/* store the pointers exported to bash in a tree*/
void * newtComponents = NULL;

static int bash_newt_compare_ptr (const void *a, const void *b) {
  if (a == b) return 0;
  if (a < b) return -1;
  return 1;
}


void bash_newt_destroy_callback(newtComponent co, void *data) {
  bash_newt_component bash_co = (bash_newt_component)data;
  /* if (bash_co->filter) free(bash_co->filter); */
  if (bash_co->destroy_callback) {
    int ret =  evalstring (savestring(bash_co->destroy_callback), NULL, 0);
    xfree(bash_co->destroy_callback);
  }
  tdelete(bash_co, &newtComponents, bash_newt_compare_ptr);
  xfree(bash_co);
}

bash_newt_component bash_newt_new(newtComponent co) {

  bash_newt_component bash_co = xmalloc(sizeof(struct bash_newt_component_struct));
  if (bash_co != NULL) {
     bash_co->co = NULL;
     bash_newt_set_newt_component(bash_co, co);
    /* save the value in the tree */
     bash_newt_component *p = (bash_newt_component *)tsearch(bash_co, &newtComponents, bash_newt_compare_ptr);
  }

  return bash_co;
}


char *  bash_newt_get_checkbox_result(bash_newt_component bash_co){
  return & bash_co->checkbok_result;

}

void bash_newt_set_newt_component(bash_newt_component bash_co, newtComponent co){
 if (bash_co != NULL) {
   if (bash_co->co != NULL) {
     newtComponentDestroy(bash_co->co);
   }
   bash_co->co = co; 
   bash_co->destroy_callback = NULL;
   if (co != NULL) {
     newtComponentAddDestroyCallback(co, bash_newt_destroy_callback, bash_co);
   }
   /* save the value in the tree */
   bash_newt_component *p = (bash_newt_component *)tsearch(bash_co, &newtComponents, bash_newt_compare_ptr);
  }

}

newtComponent bash_newt_get_newt_component(bash_newt_component bash_co){
  if (tfind(bash_co, &newtComponents, bash_newt_compare_ptr) != NULL){
    return (bash_co)->co;
  } else {
    return NULL;
  }
}


void bash_newt_set_destroy_callback(bash_newt_component bash_co,
				    const char * callback){
  if (tfind(bash_co, &newtComponents, bash_newt_compare_ptr) != NULL){
    /* only one callback at a time, I think newt works like this */
    xfree(bash_co->destroy_callback);
    bash_co->destroy_callback = savestring(callback);
  }
}

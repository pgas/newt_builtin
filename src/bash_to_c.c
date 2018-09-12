#include "bash_to_c.h" 

int to_int(WORD_LIST * args, int * result){
  intmax_t i;
  if (legal_number (args->next->word->word, &i)) {
    *result = (int) i;
    return 0;
  }
  return -1;
}
  

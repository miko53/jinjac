
#include "j_value.h"



void j_value_destroy(j_value* p)
{
  if (p->type == J_STRING)
  {
    free(p->value.type_string);
    p->value.type_string = NULL;
  }
}

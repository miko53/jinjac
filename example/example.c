
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jinjac.h"

int main(int argc, char* argv[])
{
  char* template =
  "Hello from jinjac {{ user }} !\n"
  "{% for x in data -%}\n"
  " -> data {{ x }}\n"
  "{% endfor %}\n"
  "end template\n"
  ;

  jinjac_init();

  jinjac_parameter john;
  john.type = TYPE_STRING;
  john.value.type_string = "John";

  jinjac_parameter_insert("user", &john);
  jinjac_parameter_array_insert("data", TYPE_INT, 10, 5, 8, 987, 16, 1, 5, 9, 8, 58, 6);

  char* pResult = NULL;
  int sizeResult = 0;

  jinjac_render_with_buffer(template, strlen(template), &pResult, &sizeResult);
  fprintf(stdout, "%s\n", pResult);

  free(pResult);
  jinjac_destroy();

  return EXIT_SUCCESS;
}

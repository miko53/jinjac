#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "common.h"

ast ast_root;

void ast_clean()
{
  ast_root.inError = FALSE;
  ast_root.type = 0;
  ast_root.identifier = NULL;
  ast_root.string = NULL;
}

ast* getAstRoot(void)
{
  return &ast_root;
}

char* upper(char* s)
{
  char* current;
  current = s;

  while (*current != '\0')
  {
    *current = toupper(*current);
    current++;
  }
  return s;
}

typedef struct
{
  filter_fct fct;
  const char* name;
} fct_converter;

fct_converter tab_fct_converter[] =
{
  { .fct = upper, .name = "upper" }
};


filter_fct getFunction(char* fctName)
{
  int sizeMax = sizeof(tab_fct_converter) / sizeof(fct_converter);
  int i;

  for (i = 0; i < sizeMax; i++)
  {
    if (strcmp(tab_fct_converter[i].name, fctName) == 0)
    {
      return tab_fct_converter[i].fct;
    }
  }


  return NULL;
}

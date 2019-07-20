
#ifndef _AST_H
#define _AST_H

#include "common.h"

typedef char* (*filter_fct)(char*, ...);

typedef enum
{
  AST_STRING,
  AST_FUNCTION
} ast_type;

typedef struct
{
  BOOL inError;
  char* string;
  ast_type type;
  union
  {
    filter_fct fct;
  };
} ast;

extern ast* getAstRoot(void);
extern void ast_clean(void);

filter_fct getFunction(char* fctName);


#endif /* _AST_H */

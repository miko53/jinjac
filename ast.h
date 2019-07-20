
#ifndef _AST_H
#define _AST_H

#include "common.h"

typedef enum
{
  AST_STRING
} ast_type;

typedef struct
{
  BOOL inError;
  ast_type type;
  union
  {
    char* string;
  };
} ast;

extern ast* getAstRoot(void);
extern void ast_clean(void);

#endif /* _AST_H */

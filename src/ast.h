
#ifndef _AST_H
#define _AST_H

#include "common.h"
#include "parameter.h"

typedef char* (*filter_fct)(char*, ...);

typedef enum
{
  AST_CONSTANTE,
  AST_IDENTIFIER,
  AST_STRING,
  AST_FUNCTION,
} ast_type;

typedef struct
{
  BOOL inError;
  char* identifier;
  parameter_type value;
  filter_fct fct;
  char* string; //TODO becareful!! not to be dessaloocation systematically see test_06 extension to set
  ast_type type;
} ast;

extern ast* getAstRoot(void);
extern void ast_clean(void);

filter_fct getFunction(char* fctName);

extern char* intToStr(int value);
extern char* doubleToStr(double value);


#endif /* _AST_H */

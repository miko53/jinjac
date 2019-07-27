
#ifndef _AST_H
#define _AST_H

#include "common.h"
#include "parameter.h"

typedef char* (*filter_fct)(char*, ...);

typedef enum
{
  J_STR_CONSTANTE,
  J_INTEGER,
  J_DOUBLE,
  J_IDENTIFIER,
  J_ARRAY,
  J_FUNCTION_ARGS,
  J_FUNCTION,
  
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
  char* string; //TODO be careful!! not to desallocate systematically see test_06 extension
  ast_type type;
} ast;

typedef struct
{
  ast_type type;
} JObject;

typedef struct
{
  JObject base;
  char* str_constant;
} JStringConstante;

typedef struct
{
  JObject base;
  int value;
} JInteger;

typedef struct
{
  JObject base;
  double value;
} JDouble;

typedef struct
{
  JObject base;
  char* identifier;
} JIdentifier;

typedef struct
{
  JObject base;
  char* identifier;
  int offset;
} JArray;

#define NB_MAX_ARGS   (10)

typedef struct
{
  JObject* listArgs[NB_MAX_ARGS];
  int nb_args;
} JArgs;

typedef struct
{
  JObject base;
  filter_fct function;
  JArgs argList;  
} JFunction;

extern ast* getAstRoot(void);
extern void ast_clean(void);

filter_fct getFunction(char* fctName);

#endif /* _AST_H */

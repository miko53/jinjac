#ifndef _AST_H
#define _AST_H

#include "common.h"

extern void ast_clean(void);
extern void ast_setInError(char* errorString);
extern BOOL ast_getInError(void);
extern char* ast_getStringResult(void);

extern int ast_insert_constante(char* name);
extern int ast_insert_identifier(char* name);
extern int ast_insert_integer(int i);
extern int ast_insert_double(double d);
extern int ast_insert_boolean(BOOL b);
extern int ast_insert_function(char* fct);
extern int ast_create_array_on_top(char* name);
extern int ast_create_function_args_from_top(void);
extern int ast_insert_function_args(void);

extern int ast_do_operation(char mathOperation);

extern int ast_dump_stack(void);

extern int ast_execute_function(void);

#endif /* _AST_H */

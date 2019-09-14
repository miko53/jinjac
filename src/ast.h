/*
 * Copyright (c) 2019 miko53
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _AST_H
#define _AST_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  IN_ERROR = -1,
  OK_DONE,
  FOR_STATEMENT,
  END_FOR_STATEMENT
} ast_status;

extern void ast_init(void);
extern void ast_clean(void);
extern void ast_removeLastResultItem(void);

extern void ast_setInError(char* errorString);

extern ast_status ast_getStatus(void);

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

extern int ast_create_for_stmt(char* identifierName);
extern BOOL ast_setBeginOfForStatement(long offset);
extern int ast_create_end_for_stmt(void);
extern BOOL ast_executeEndForStmt(long int* returnOffset);
extern BOOL ast_forStmtIsLineToBeIgnored();

#ifdef __cplusplus
}
#endif

#endif /* _AST_H */

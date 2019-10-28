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
#ifndef _JOBJECT_H
#define _JOBJECT_H

#include "common.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  J_STR_CONSTANTE,
  J_INTEGER,
  J_DOUBLE,
  J_BOOLEAN,
  J_IDENTIFIER,
  J_ARRAY,
  J_FUNCTION_ARGS,
  J_FUNCTION,
  J_RANGE,
  J_FOR,
  J_END_FOR,
  J_IF,
  J_ELSE,
  J_END_IF,
  J_LIST
} jobject_type;

typedef enum
{
  AST_EQUAL,
  AST_DIFFERENT,
  AST_HIGH_AND_EQUAL_THAN,
  AST_HIGH_THAN,
  AST_LOWER_AND_EQUAL_THAN,
  AST_LOWER_THAN,
} jobject_condition;

typedef enum
{
  AST_OR,
  AST_AND,
  AST_NOT,
  AST_IS
} jobject_logical_condition;

typedef struct JObjects
{
  jobject_type type;
  void (*delete)(struct JObjects* o);
  BOOL (*getValue)(struct JObjects* pObject, jinjac_parameter* param);
  BOOL (*toBoolean)(struct JObjects* pObject);
} JObject;

typedef struct
{
  JObject base;
  char* str_constant;
} JStringConstante;

typedef struct
{
  JObject base;
  int32_t value;
} JInteger;

typedef struct
{
  JObject base;
  double value;
} JDouble;

typedef struct
{
  JObject base;
  BOOL value;
} JBoolean;

typedef struct
{
  JObject base;
  char* identifier;
} JIdentifier;

typedef struct
{
  JObject base;
  char* identifier;
  int32_t offset;
} JArray;

typedef struct
{
  JObject base;
  JObject* sequencedObject;
  int32_t start;
  int32_t stop;
  int32_t step;
  int32_t currentIndex;
} JRange;

typedef struct
{
  JObject base;
  char* identifierOfIndex;
  JRange* sequencing;
  int64_t startOffset;
  int32_t startLine;
  BOOL bStripWhiteSpace;
} JFor;


typedef struct
{
  JObject base;
} JEndFor;

typedef struct
{
  JObject base;
  JObject* condition;
} JIF;

typedef struct
{
  JObject base;
} JEndIf;

typedef struct
{
  JObject base;
} JElse;

typedef struct JListItemS
{
  JObject* object;
  struct JListItemS* next;
} JListItem;

typedef struct
{
  JObject base;
  BOOL isTuple;
  JListItem* list;
} JList;

//constructor
extern JObject* JStringConstante_new(char* name);
extern JObject* JIdentifier_new(char* name);
extern JObject* JInteger_new(int32_t i);
extern JObject* JDouble_new(double d);
extern JObject* JBoolean_new(BOOL b);
extern JObject* JFunction_new(char* fct);
extern JObject* JArgs_new(void);
extern JObject* JArray_new(char* name, int32_t offset);
extern JObject* JFor_new(char* nameIdentifier, JRange* sequence);
extern JObject* JEndFor_new(void);

//JObject
extern char* JObject_toString(JObject* pObject);
extern BOOL JObject_getValue(JObject* pObject, jinjac_parameter* param);
extern int32_t JObject_toInteger(JObject* obj);
extern BOOL JObject_toBoolean(JObject* pObject);

extern JObject* JObject_doOperation(JObject* op1, JObject* op2, char mathOperation);
extern void JObject_delete(JObject* pObject);
extern JRange* JObject_toRange(JObject* pObject);

//JFor
extern J_STATUS JFor_setStartPoint(JFor* obj, int64_t offset, int32_t noLine, BOOL bStripWhiteSpace);
extern J_STATUS JFor_createIndexParameter(JFor* obj);
extern BOOL JFor_isDone(JFor* obj);

//JRange
extern JObject* JRange_new(JObject* objectToBeSequenced, int32_t start, int32_t stop, int32_t step);
extern BOOL JRange_step(JRange* obj, char* indexIdentifierName);

extern JObject* JObject_execComparison(JObject* op1, JObject* op2, jobject_condition condition);

//JIF
extern JObject* JIF_new(JObject* condition);
extern BOOL JIF_getIfConditionIsActive(JIF* pIf);
extern JObject* JEndIf_new(void);

extern JObject* JElse_new(void);


#ifdef __cplusplus
}
#endif

#endif /* _JOBJECT_H */

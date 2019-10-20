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

#ifndef _JFUNCTION_H
#define _JFUNCTION_H

#include "jobject.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  FCT_CAPITALIZE,
  FCT_CENTER,
  FCT_FORMAT,
  FCT_JOIN,
  FCT_LOWER,
  FCT_UPPER,
  FCT_RANGE,
  FCT_TITLE,
  FCT_TRIM,
  FCT_TRUNCATE
} fct_id;

#define NB_MAX_ARGS   (10)

typedef struct
{
  JObject base;
  JObject* listArgs[NB_MAX_ARGS];
  int32_t nb_args;
} JArgs;

typedef struct
{
  JObject base;
  fct_id functionID;
  JArgs* argList;
} JFunction;


//JFunction
extern JObject* JFunction_execute(JFunction* f, JObject* pCurrentObject);
extern J_STATUS JArgs_insert_args(JArgs* obj, JObject* argToInsert);

#ifdef __cplusplus
}
#endif

#endif /* _JFUNCTION_H */

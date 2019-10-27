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
#ifndef _JINJAC_H
#define _JINJAC_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  J_OK = 0,
  J_ERROR = -1
} J_STATUS;

typedef enum
{
  TYPE_STRING,
  TYPE_INT,
  TYPE_DOUBLE,
} jinjac_parameter_type;

typedef union
{
  char* type_string;
  int32_t type_int;
  double type_double;
} jinjac_parameter_value;

typedef struct
{
  jinjac_parameter_type type;
  jinjac_parameter_value value;
} jinjac_parameter;

extern void jinjac_init(void);
extern void jinjac_destroy(void);

extern void jinjac_dbg_parse_string(char* string);
extern void jinjac_render_with_file(FILE* in, FILE* out);
extern void jinjac_render_with_buffer(char* in, int32_t sizeIn, char** pOut, int32_t* pSizeOut);

extern J_STATUS jinjac_parameter_insert(char* key, jinjac_parameter* param);
extern J_STATUS jinjac_parameter_array_insert(char* key, jinjac_parameter_type type, int32_t nbValue, ...);
extern void jinjac_parameter_delete_all(void);

typedef struct
{
  int (*search)(char* key, int32_t* privKey, int* isArray);
  J_STATUS (*get)(int32_t privKey, jinjac_parameter* param);
  int (*array_getProperties)(int32_t privKey, jinjac_parameter_type* type, int32_t* nbItem);
  J_STATUS (*array_getValue)(int32_t privKey, int32_t offset, jinjac_parameter_value* v);
} jinjac_parameter_callback;

extern J_STATUS jinjac_parameter_registerUserParameter(jinjac_parameter_callback* callback);


#ifdef __cplusplus
}
#endif

#endif /* _JINJAC_H */

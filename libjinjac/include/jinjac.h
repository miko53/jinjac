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
#ifndef _JINJA_H
#define _JINJA_H

#include <stdio.h>

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
} parameter_type;

typedef union
{
  char* type_string;
  int type_int;
  double type_double;
} parameter_value;

typedef struct
{
  parameter_type type;
  parameter_value value;
} parameter;

extern void jinjac_init(void);
extern void jinjac_destroy(void);

extern void jinjac_parse_string(char* string);
extern void jinjac_parse_file(FILE* in, FILE* out);

extern J_STATUS jinjac_parameter_insert(char* key, parameter* param);
extern J_STATUS jinjac_parameter_array_insert(char* key, parameter_type type, int nbValue, ...);
extern void jinjac_parameter_delete_all(void);

#ifdef __cplusplus
}
#endif

#endif /* _JINJA_H */
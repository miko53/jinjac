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

#ifndef _JINJAC_STREAM_H
#define _JINJAC_STREAM_H

#include "common.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  JINJAC_PARSE_FILE,
  JINJAC_PARSE_BUFFER
} jinjac_parsing_type;

typedef struct
{
  char* buffer;
  int32_t size;
} BUFFER;

struct jinjac_streamS;

typedef struct
{
  int32_t (*fgetc)(struct jinjac_streamS* ctxt);
  int32_t (*feof)(struct jinjac_streamS* ctxt);
  int32_t (*fputc)(struct jinjac_streamS* ctxt, int32_t c);
  int32_t (*fputs)(struct jinjac_streamS* ctxt, char* s);
  int32_t (*fseek)(struct jinjac_streamS* ctxt, int64_t offset);
  int64_t (*ftell)(struct jinjac_streamS* ctxt);
} steam_ops;

typedef struct jinjac_streamS
{
  jinjac_parsing_type type;
  union
  {
    FILE* file;
    BUFFER* buffer;
  };
  steam_ops ops;
} jinjac_stream;

extern int32_t jinjac_stream_initFile(jinjac_stream* ctxt, FILE* file);
extern int32_t jinjac_stream_initBuffer(jinjac_stream* ctxt, BUFFER* buffer);

#ifdef __cplusplus
}
#endif

#endif /* _JINJAC_STREAM_H */



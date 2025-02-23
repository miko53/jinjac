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

#ifndef _COMMON_H
#define _COMMON_H

#include <assert.h>
#include <stdlib.h>
#include "trace.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef JINJAC_DEBUG
#define dbg_print(...)       fprintf(stdout, __VA_ARGS__)
#define ASSERT(expr)         assert(expr)
#else
#define dbg_print(...)
#define ASSERT(expr)         ((void) 0)
#endif /* JINJAC_DEBUG*/

#ifdef TRACE
#define trace(...)           print_trace(__FILE__, __LINE__, __VA_ARGS__)
#else
#define trace(...)           ((void) (0))
#endif

#define error(level, ...)    print_error(level, __VA_ARGS__)

typedef enum
{
  FALSE,
  TRUE
} BOOL;

#define STATIC      static
#define NEW(obj)    malloc(sizeof(obj))

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H */

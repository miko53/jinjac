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
#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8_t* buffer;
  int32_t size;
  int64_t pReadOffset;
  int64_t pWriteOffset;
  BOOL    bEndOfFile;
} BUFFER;

extern int32_t buffer_init(BUFFER* b, uint8_t* pData, int32_t size);
extern int32_t buffer_getc(BUFFER* b);
extern int32_t buffer_eof(BUFFER* b);
extern int32_t buffer_putc(BUFFER* b, int32_t c);
extern int32_t buffer_puts(BUFFER* b, char* str);
extern int32_t buffer_seek(BUFFER* b, int64_t offset);
extern int64_t buffer_tell(BUFFER* b);

#ifdef __cplusplus
}
#endif


#endif /* _BUFFER_H */

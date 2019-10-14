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

#include "buffer.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

int32_t buffer_init(BUFFER* b, uint8_t* pData, int32_t size)
{
  ASSERT(b != NULL);
  b->size = size;
  b->buffer = pData;
  b->pReadOffset = 0;
  b->pWriteOffset = 0;
  b->bEndOfFile = FALSE;
  return 0;
}

int32_t buffer_getc(BUFFER* b)
{
  ASSERT(b != NULL);
  int32_t c;
  if (b->pReadOffset < b->size)
  {
    c = b->buffer[b->pReadOffset];
    b->pReadOffset++;
    b->pWriteOffset = b->pReadOffset;
  }
  else
  {
    c = -1;
    b->bEndOfFile = TRUE;
  }

  return c;
}

int32_t buffer_eof(BUFFER* b)
{
  ASSERT(b != NULL);
  return b->bEndOfFile;
}

STATIC int32_t buffer_allocate(BUFFER* b, int32_t nbToAdd)
{
  int32_t rc;
  int32_t minToAllocate;

  minToAllocate = 50;
  rc = -1;

  if (nbToAdd > minToAllocate)
  {
    minToAllocate = nbToAdd;
  }

  if (((b->size - minToAllocate) <  (b->size * 2)) && (b->size != 0))
  {
    minToAllocate = (b->size * 2);
  }

  if ((b->buffer == NULL) || (b->size == 0))
  {
    //trace("buffer allocation %d bytes\n", minToAllocate);
    b->buffer = malloc(minToAllocate);
    if (b->buffer != NULL)
    {
      b->size = minToAllocate;
      rc = 0;
    }
  }
  else
  {
    //trace("buffer reallocation %d bytes\n", minToAllocate);
    uint8_t* newBuffer = realloc(b->buffer, minToAllocate);
    if (newBuffer != NULL)
    {
      b->buffer = newBuffer;
      b->size = minToAllocate;
      rc = 0;
    }
  }

  return rc;
}

int32_t buffer_putc(BUFFER* b, int32_t c)
{
  ASSERT(b != NULL);
  int32_t rc;
  rc = -1;

  if ((b->pWriteOffset + 1) >= b->size)
  {
    //need allocate memory
    rc = buffer_allocate(b, b->size + c);
    if (rc == 0)
    {
      b->buffer[b->pWriteOffset] = c;
      rc = c;
      b->pWriteOffset++;
      b->pReadOffset = b->pWriteOffset;
    }
  }
  else
  {
    b->buffer[b->pWriteOffset] = c;
    rc = c;
    b->pWriteOffset++;
    b->pReadOffset = b->pWriteOffset;
  }

  return rc;
}


int32_t buffer_puts(BUFFER* b, char* str)
{
  ASSERT(b != NULL);
  ASSERT(str != NULL);

  int32_t rc;
  int32_t size;
  rc = -1;
  size = strlen(str);
  if ((b->pWriteOffset + size) >= b->size)
  {
    //need allocate memory
    rc = buffer_allocate(b, b->size + size);
    if (rc == 0)
    {
      memcpy(&b->buffer[b->pWriteOffset], str, size);
      b->pWriteOffset += size;
      b->pReadOffset = b->pWriteOffset;
      rc = size;
    }
  }
  else
  {
    memcpy(&b->buffer[b->pWriteOffset], str, size);
    b->pWriteOffset += size;
    b->pReadOffset = b->pWriteOffset;
    rc = size;
  }

  return rc;
}


int32_t buffer_seek(BUFFER* b, int64_t offset)
{
  ASSERT(b != NULL);
  int32_t rc;
  rc = -1;
  if (offset < b->size)
  {
    b->pReadOffset = offset;
    b->pWriteOffset = offset;
    b->bEndOfFile = FALSE;
    rc = 0;
  }

  return rc;
}


int64_t buffer_tell(BUFFER* b)
{
  ASSERT(b != NULL);
  return b->pReadOffset;
}

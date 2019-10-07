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

#include "jinjac_stream.h"
#include "buffer.h"

STATIC int32_t jinjac_file_readNextChar(jinjac_stream* ctxt);
STATIC int32_t jinjac_file_endOfFile(jinjac_stream* ctxt);
STATIC int32_t jinjac_file_writeChar(jinjac_stream* ctxt, int32_t c);
STATIC int32_t jinjac_file_writeString(jinjac_stream* ctxt, char* s);
STATIC int32_t jinjac_file_seek(jinjac_stream* ctxt, int64_t offset);
STATIC int64_t jinjac_file_tell(jinjac_stream* ctxt);

STATIC int32_t jinjac_buffer_readNextChar(jinjac_stream* ctxt);
STATIC int32_t jinjac_buffer_endOfFile(jinjac_stream* ctxt);
STATIC int32_t jinjac_buffer_writeChar(jinjac_stream* ctxt, int32_t c);
STATIC int32_t jinjac_buffer_writeString(jinjac_stream* ctxt, char* s);
STATIC int32_t jinjac_buffer_seek(jinjac_stream* ctxt, int64_t offset);
STATIC int64_t jinjac_buffer_tell(jinjac_stream* ctxt);


const steam_ops stream_ops_file =
{
  .fgetc = jinjac_file_readNextChar,
  .feof = jinjac_file_endOfFile,
  .fputc = jinjac_file_writeChar,
  .fputs = jinjac_file_writeString,
  .fseek = jinjac_file_seek,
  .ftell = jinjac_file_tell
};

const steam_ops stream_ops_buffer =
{
  .fgetc = jinjac_buffer_readNextChar,
  .feof = jinjac_buffer_endOfFile,
  .fputc = jinjac_buffer_writeChar,
  .fputs = jinjac_buffer_writeString,
  .fseek = jinjac_buffer_seek,
  .ftell = jinjac_buffer_tell
};


int32_t jinjac_stream_initBuffer(jinjac_stream* ctxt, BUFFER* buffer)
{
  ASSERT(ctxt != NULL);

  ctxt->type = JINJAC_PARSE_BUFFER;
  ctxt->buffer = buffer;
  ctxt->ops = stream_ops_buffer;
  return -1;
}


int32_t jinjac_stream_initFile(jinjac_stream* ctxt, FILE* file)
{
  ASSERT(ctxt != NULL);

  ctxt->type = JINJAC_PARSE_FILE;
  ctxt->file = file;
  ctxt->ops = stream_ops_file;
  return 0;
}

int32_t jinjac_file_readNextChar(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return fgetc(ctxt->file);
}

int32_t jinjac_file_endOfFile(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return feof(ctxt->file);
}

int32_t jinjac_file_writeChar(jinjac_stream* ctxt, int32_t c)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return fputc(c, ctxt->file);
}

int32_t jinjac_file_writeString(jinjac_stream* ctxt, char* s)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return fputs(s, ctxt->file);
}


int32_t jinjac_file_seek(jinjac_stream* ctxt, int64_t offset)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return fseek(ctxt->file, offset, SEEK_SET);
}


int64_t jinjac_file_tell(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_FILE);
  return ftell(ctxt->file);
}


//TODO
int32_t jinjac_buffer_readNextChar(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_getc(ctxt->buffer);
}

int32_t jinjac_buffer_endOfFile(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_eof(ctxt->buffer);
}

int32_t jinjac_buffer_writeChar(jinjac_stream* ctxt, int32_t c)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_putc(ctxt->buffer, c);
}

int32_t jinjac_buffer_writeString(jinjac_stream* ctxt, char* s)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_puts(ctxt->buffer, s);
}

int32_t jinjac_buffer_seek(jinjac_stream* ctxt, int64_t offset)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_seek(ctxt->buffer, offset);
}


int64_t jinjac_buffer_tell(jinjac_stream* ctxt)
{
  ASSERT(ctxt->type == JINJAC_PARSE_BUFFER);
  return buffer_tell(ctxt->buffer);
}


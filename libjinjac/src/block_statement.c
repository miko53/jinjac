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

#include "block_statement.h"

#define BLOCK_NB_MAX      (50)

typedef struct
{
  BOOL bIsBlockActive;
  BOOL bIsConditionActive;
  ast_status blockType;
} block;

STATIC block block_stack[BLOCK_NB_MAX];
STATIC int32_t block_level;

BOOL block_statement_isCurrentBlockActive(void)
{
  BOOL b;
  b = FALSE;

  if (block_level == 0)
  {
    b = TRUE;
  }
  else
  {
    b = block_stack[block_level - 1].bIsBlockActive;
  }

  return b;
}

BOOL block_statement_isCurrentBlockConditionActive(void)
{
  BOOL b;
  b = FALSE;

  if (block_level == 0)
  {
    b = TRUE;
  }
  else
  {
    if (block_stack[block_level - 1].bIsBlockActive)
    {
      b = block_stack[block_level - 1].bIsConditionActive;
    }
  }

  return b;
}

ast_status block_statement_getCurrentBlockType(void)
{
  ast_status s;
  s = IN_ERROR;

  if (block_level != 0)
  {
    s = block_stack[block_level - 1 ].blockType;
  }

  return s;
}


BOOL block_statement_createNewBlock(ast_status blockType, BOOL bIsBlockActive, BOOL bIsConditionActive)
{
  BOOL bOk;
  bOk = FALSE;
  if (block_level < BLOCK_NB_MAX)
  {
    block_stack[block_level].blockType = blockType;
    block_stack[block_level].bIsBlockActive = bIsBlockActive;
    block_stack[block_level].bIsConditionActive = bIsConditionActive;
    block_level++;
    bOk = TRUE;
  }
  else
  {
    ASSERT(FALSE);
  }

  return bOk;
}

BOOL block_statement_removeCurrrent()
{
  BOOL bOk;
  bOk = FALSE;

  ASSERT(block_level >= 1);
  if (block_level >= 1)
  {
    block_level--;
    bOk = TRUE;
  }

  return bOk;
}


BOOL block_statement_setConditionActiveOfCurrentBlock(BOOL condition)
{
  BOOL bOk;
  bOk = FALSE;
  ASSERT(block_level >= 1);
  if (block_level >= 1)
  {
    block_stack[block_level - 1].bIsConditionActive = condition;
    bOk = TRUE;
  }

  return bOk;
}

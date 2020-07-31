#include "code.h"
#include <iostream>
#include <cstdarg>
#include <cassert>
#include "verbose.h"

Code::Code()
{

}

Code::~Code()
{

}

static const char* vm_byte_code[] =
{
#define OPCODE(x) #x,
#include "ops.h"
#undef OPCODE
};

uint32_t Code::insertvStatement(VM_BYTE_CODE opcode, va_list args)
{
  int32_t arg1;
  double arg2;
  uint32_t offset;

  //std::cout << vm_byte_code[opcode] << "  ";
  verbose_print(1, "%s ", vm_byte_code[opcode]);
  offset = m_bytecode.insert((int8_t) opcode);
  switch (opcode)
  {
    case OP_COPY_STRING:
      arg1 = va_arg(args, int32_t);
      //std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_NOP: //no argument
    case OP_HALT:
    case OP_EVAL:
    case OP_PUSH_NIL:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_GET_AT:
    case OP_CREATE_ITERATOR:
    case OP_ITERATOR_LOAD_FIRST:
    case OP_POP:
    case OP_ITERATOR_TEST_END_REACHED:
    case OP_ITERATOR_INCR:
    case OP_ITERATOR_LOAD_NEXT:
    case OP_TEST_EQ:
    case OP_TEST_NEQ:
    case OP_TEST_GE:
    case OP_TEST_GT:
    case OP_TEST_LE:
    case OP_TEST_LT:
    case OP_TEST_OR:
    case OP_TEST_AND:
    case OP_TEST_NOT:
    case OP_TEST_IS:
    case OP_CREATE_TUPLE:
    case OP_CREATE_ARRAY:
      break;

    case OP_PUSH_STRING:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_CALL_BUILDIN_FCT:
      {
        int32_t nbArgs;
        arg1 = va_arg(args, int32_t);
        nbArgs = va_arg(args, int32_t);
        //         std::cout << arg1 << " " << nbArgs;
        verbose_print(1, "%d %d", arg1, nbArgs);
        m_bytecode.insert(arg1);
        m_bytecode.insert(nbArgs);
      }
      break;

    case OP_LOAD_SYMBOL:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_LOAD_INTEGER:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_LOAD_DOUBLE:
      arg2 = va_arg(args, double);
      //       std::cout << arg2;
      verbose_print(1, "%f", arg2);
      m_bytecode.insert(arg2);
      break;

    case OP_CREATE_SYMBOL:
    case OP_DELETE_SYMBOL:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_SET_SYMBOL:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    case OP_B_TRUE:
    case OP_JUMP:
    case OP_B_FALSE:
      arg1 = va_arg(args, int32_t);
      //       std::cout << arg1;
      verbose_print(1, "%d", arg1);
      m_bytecode.insert(arg1);
      break;

    default:
      assert(false);
      break;
  }

  verbose_print(1, "\n");
  //   std::cout << std::endl;

  va_end(args);
  return offset;
}

uint32_t Code::insertStatement(VM_BYTE_CODE opcode, ...)
{
  va_list args;
  va_start(args, opcode);
  uint32_t rc;
  rc =  insertvStatement(opcode, args);
  va_end(args);
  return rc;
}

uint32_t Code::replaceStatement(uint32_t offset, VM_BYTE_CODE opcode, ...)
{
  va_list args;
  va_start(args, opcode);

  int32_t currentOffset = m_bytecode.getCurrent();
  m_bytecode.setCurrent(offset);
  insertvStatement(opcode, args);
  m_bytecode.setCurrent(currentOffset);
  va_end(args);
  return 0;
}

void Code::write(std::ofstream* outFile)
{
  m_bytecode.write(outFile);
}

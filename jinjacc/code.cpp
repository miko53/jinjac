#include "code.h"
#include <iostream>
#include <cstdarg>
#include <cassert>
 
Code::Code()
{

}

Code::~Code()
{

}

const char* vm_byte_code[] = 
{
  "OP_COPY_STRING",
  "OP_NOP",
  "OP_HALT",
  "OP_PUSH_STRING",
  "OP_PUSH_NIL",
  "OP_EVAL",
  "OP_CALL_BUILDIN_FCT",
  "OP_LOAD_SYMBOL",
  "OP_LOAD_INTEGER",
  "OP_LOAD_DOUBLE",
  "OP_ADD",
  "OP_SUB",
  "OP_MUL",
  "OP_DIV",
  "OP_GET_AT",
  "OP_CREATE_SYMBOL",
  "OP_DELETE_SYMBOL",
  "OP_SET_SYMBOL",
  "OP_CREATE_ITERATOR",
  "OP_ITERATOR_LOAD_FIRST",
  "OP_POP",
  "OP_ITERATOR_TEST_END_REACHED",
  "OP_B_TRUE",
  "OP_B_FALSE",
  "OP_JUMP",
  "OP_ITERATOR_INCR",
  "OP_ITERATOR_LOAD_NEXT",
  "OP_TEST_EQ",
  "OP_TEST_NEQ",
  "OP_TEST_GE",
  "OP_TEST_GT",
  "OP_TEST_LE",
  "OP_TEST_LT",
  "OP_TEST_OR",
  "OP_TEST_AND",
  "OP_TEST_NOT",
  "OP_TEST_IS",
  "OP_CREATE_TUPLE",
  "OP_CREATE_ARRAY",
};

uint32_t Code::insertvStatement(VM_BYTE_CODE opcode, va_list args)
{
  int32_t arg1;
  double arg2;
  uint32_t offset;
  
  std::cout << vm_byte_code[opcode] << "  ";
  offset = m_bytecode.insert((int8_t) opcode);
  switch(opcode)
  {
    case OP_COPY_STRING:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
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
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
                  
    case OP_CALL_BUILDIN_FCT:
    {
      int32_t nbArgs;
      arg1 = va_arg(args, int32_t);
      nbArgs = va_arg(args, int32_t);
      std::cout << arg1 << " " << nbArgs;
      m_bytecode.insert(arg1);
      m_bytecode.insert(nbArgs);
    }
      break;
    
    case OP_LOAD_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
    
    case OP_LOAD_INTEGER:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
      
    case OP_LOAD_DOUBLE:
      arg2 = va_arg(args, double);
      std::cout << arg2;
      m_bytecode.insert(arg2);
      break;
      
    case OP_CREATE_SYMBOL:
    case OP_DELETE_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
    
    case OP_SET_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;

    case OP_B_TRUE:
    case OP_JUMP:
    case OP_B_FALSE:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
      
    default:
      assert(false);
      break;
  }
  
  std::cout << std::endl;
  
  va_end(args);
  return offset;  
}

uint32_t Code::insertStatement(VM_BYTE_CODE opcode, ...)
{
  va_list args;
  va_start(args, opcode);  
  int32_t arg1;
  double arg2;
  uint32_t offset;
  
  std::cout << vm_byte_code[opcode] << "  ";
  offset = m_bytecode.insert((int8_t) opcode);
  switch(opcode)
  {
    case OP_COPY_STRING:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
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
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
                  
    case OP_CALL_BUILDIN_FCT:
    {
      int32_t nbArgs;
      arg1 = va_arg(args, int32_t);
      nbArgs = va_arg(args, int32_t);
      std::cout << arg1 << " " << nbArgs;
      m_bytecode.insert(arg1);
      m_bytecode.insert(nbArgs);
    }
      break;
    
    case OP_LOAD_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
    
    case OP_LOAD_INTEGER:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
      
    case OP_LOAD_DOUBLE:
      arg2 = va_arg(args, double);
      std::cout << arg2;
      m_bytecode.insert(arg2);
      break;
      
    case OP_CREATE_SYMBOL:
    case OP_DELETE_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
    
    case OP_SET_SYMBOL:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;

    case OP_B_TRUE:
    case OP_JUMP:
    case OP_B_FALSE:
      arg1 = va_arg(args, int32_t);
      std::cout << arg1;
      m_bytecode.insert(arg1);
      break;
      
    default:
      assert(false);
      break;
  }
  
  std::cout << std::endl;
  
  va_end(args);
  return offset;
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

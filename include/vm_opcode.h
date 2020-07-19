#ifndef VM_OPCODE_H
#define VM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  FCT_CAPITALIZE,
  FCT_CENTER,
  FCT_FORMAT,
  FCT_JOIN,
  FCT_LOWER,
  FCT_UPPER,
  FCT_RANGE,
  FCT_TITLE,
  FCT_TRIM,
  FCT_TRUNCATE
} buildin_fct_id;

extern const char* vm_buildin_fct[];

typedef enum
{
  OP_COPY_STRING,      /// @offset static data, copy string to output stream
  OP_NOP,
  OP_HALT,             /// terminate vm
  OP_PUSH_STRING,      /// @offset of data push it on stack
  OP_PUSH_NIL,         /// insert @NULL pointer or @integer 0 into stack
  OP_EVAL,             /// take j_value under top stack, convert to string and copy string to output stream
  OP_CALL_BUILDIN_FCT, /// call buildin function @offset get the name of the function @nb_arg give the nb of argument
  OP_LOAD_SYMBOL,      /// @offset name of identifier get object in stack
  OP_LOAD_INTEGER,     /// @offset integer value pushed into stack
  OP_LOAD_DOUBLE,      /// @offset double vale pushed into stack
  OP_ADD,              /// do addition of value in stack (two) remove them and push result in stack
  OP_SUB,              /// do subtraction of value in stack (two) remove them and push result in stack
  OP_MUL,              /// do multiplication of value in stack (two) remove them and push result in stack
  OP_DIV,              /// do division of value in stack (two) remove them and push result in stack
  OP_GET_AT,           /// push onto stack the object given by object on stack at offset (given at @stack+1) @object --> reference @offset (convert in value)
  OP_CREATE_SYMBOL,    /// @offset of identifier name
  OP_DELETE_SYMBOL,    /// @offset of identifier name
  OP_SET_SYMBOL,       /// @offset of identifier name, value is in top of @stack
  OP_CREATE_ITERATOR,  /// create a iterator according to items in first level of @stack
  OP_ITERATOR_LOAD_FIRST, /// from iterator obj from @stack, push the first items into @stack
  OP_POP,
  OP_ITERATOR_TEST_END_REACHED, /// check if end of iterator is reach iterator is in top of @stack, it push a boolean result in @tack
  OP_B_TRUE,
  OP_B_FALSE,
  OP_JUMP,
  OP_ITERATOR_INCR,
  OP_ITERATOR_LOAD_NEXT,
  OP_TEST_EQ,
  OP_TEST_NEQ,
  OP_TEST_GE,
  OP_TEST_GT,
  OP_TEST_LE,
  OP_TEST_LT,
  OP_TEST_OR,
  OP_TEST_AND,
  OP_TEST_NOT,
  OP_TEST_IS,
  OP_CREATE_TUPLE,
  OP_CREATE_ARRAY,
} VM_BYTE_CODE;

extern const char* vm_byte_code[];

#ifdef __cplusplus
}
#endif

#endif /* VM_OPCODE_H */

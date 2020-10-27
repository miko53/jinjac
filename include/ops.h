
OPCODE(COPY_STRING)      /// @offset static data) copy string to output stream
OPCODE(NOP)
OPCODE(HALT)             /// terminate vm
OPCODE(PUSH_STRING)      /// @offset of data push it on stack
OPCODE(PUSH_NIL)         /// insert @NULL pointer or @integer 0 into stack
OPCODE(EVAL)             /// take j_value under top stack) convert to string and copy string to output stream
OPCODE(CALL_BUILDIN_FCT) /// call buildin function @offset get the name of the function @nb_arg give the nb of argument
OPCODE(LOAD_SYMBOL)      /// @offset name of identifier get object in stack
OPCODE(LOAD_INTEGER)     /// @offset integer value pushed into stack
OPCODE(LOAD_DOUBLE)      /// @offset double vale pushed into stack
OPCODE(ADD)              /// do addition of value in stack (two) remove them and push result in stack
OPCODE(SUB)              /// do subtraction of value in stack (two) remove them and push result in stack
OPCODE(MUL)              /// do multiplication of value in stack (two) remove them and push result in stack
OPCODE(DIV)              /// do division of value in stack (two) remove them and push result in stack
OPCODE(GET_AT)           /// push onto stack the object given by object on stack at offset (given at @stack+1) @object --> reference @offset (convert in value)
OPCODE(CREATE_SYMBOL)    /// @offset of identifier name
OPCODE(DELETE_SYMBOL)    /// @offset of identifier name
OPCODE(SET_SYMBOL)       /// @offset of identifier name) value is in top of @stack
OPCODE(CREATE_ITERATOR)  /// create a iterator according to items in first level of @stack
OPCODE(ITERATOR_LOAD_FIRST) /// from iterator obj from @stack) push the first items into @stack
OPCODE(POP)
OPCODE(ITERATOR_TEST_END_REACHED) /// check if end of iterator is reach iterator is in top of @stack) it push a boolean result in @tack
OPCODE(B_TRUE)
OPCODE(B_FALSE)
OPCODE(JUMP)
OPCODE(ITERATOR_INCR)
OPCODE(ITERATOR_LOAD_NEXT)
OPCODE(TEST_EQ)
OPCODE(TEST_NEQ)
OPCODE(TEST_GE)
OPCODE(TEST_GT)
OPCODE(TEST_LE)
OPCODE(TEST_LT)
OPCODE(TEST_OR)
OPCODE(TEST_AND)
OPCODE(TEST_NOT)
OPCODE(TEST_IS)
OPCODE(CREATE_TUPLE)
OPCODE(CREATE_ARRAY)


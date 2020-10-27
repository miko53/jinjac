#ifndef CODE_H
#define CODE_H

#include "vm_opcode.h"
#include <stdint.h>
#include "buffer.h"
#include <iostream>

class Code
{
public:
    Code();
    virtual ~Code();
    uint32_t insertStatement(VM_BYTE_CODE opcode, ...);
    uint32_t insertvStatement(VM_BYTE_CODE opcode, va_list args);
    uint32_t replaceStatement(uint32_t offest, VM_BYTE_CODE opcode, ...);
    void write(std::ofstream* outFile);
    uint32_t getSize() { return m_bytecode.getSize(); }
    uint32_t getCurrent() { return m_bytecode.getCurrent(); }
    
protected:
    Buffer m_bytecode;
};

#endif // CODE_H

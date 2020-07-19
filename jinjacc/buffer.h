#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <string>
#include <iostream>

class Buffer
{
public:
    Buffer();
    virtual ~Buffer();
    int32_t insertString(const std::string& s);
    int32_t insert(const int8_t i);
    int32_t insert(const int16_t i);
    int32_t insert(const int32_t i);
    int32_t insert(const double d);
    
    void write(std::ofstream* outFile);
    
    uint32_t getSize(void) { return m_current; }
    uint32_t getCurrent(void) { return m_current; }
    void setCurrent(int32_t currentOffset) 
    { 
      if ((uint32_t) currentOffset < m_capacity) 
        m_current = currentOffset; 
    }
    
private:
    void checkAndReallocateSize(uint32_t size);
    void reallocate(uint32_t minToAdd);
    
protected:
    int32_t m_current;
    uint32_t m_capacity;
    uint8_t* m_data;
};

#endif // BUFFER_H

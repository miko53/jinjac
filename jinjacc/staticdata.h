#ifndef STATICDATA_H
#define STATICDATA_H

#include <map>
#include <string>
#include <iostream>
#include "buffer.h"

class StaticData
{
public:
    StaticData();
    virtual ~StaticData();
    
    int32_t insertString(const std::string& s);
    void write(std::ofstream* outFile);
    uint32_t getSize(void) { return m_dataStorage.getSize(); }
    
protected:
  std::map< std::string /*value*/, int32_t /* offset */> m_stringData;
  std::map< int32_t /*value*/, int32_t /* offset */> m_integer32Data;
  std::map< double /*value*/, int32_t /* offset */> m_doubleData;
  
  Buffer m_dataStorage;
  
private:
  
};

#endif // STATICDATA_H

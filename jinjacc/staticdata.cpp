#include "staticdata.h"


StaticData::StaticData()
{

}

StaticData::~StaticData()
{

}

int32_t StaticData::insertString(const std::string& s)
{
  std::map< std::string, int32_t>::iterator i;
  int32_t offset;
  i = m_stringData.find(s);
  if (i == m_stringData.end())
  {
    //not find insert it.
    offset = m_dataStorage.insertString(s);
    m_stringData[s] = offset;
  }
  else
  {
    offset = i->second;
  }
   
  return offset;
}

void StaticData::write(std::ofstream* outFile)
{
  m_dataStorage.write(outFile);
}


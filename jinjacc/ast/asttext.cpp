#include "asttext.h"
#include <assert.h>
#include <iostream>
#include "vm_opcode.h"
#include "jinjac_compiler.h"

AstText::AstText(std::string& s)
{
  m_text = s;
}

AstText::~AstText()
{

}

//TODO to be put in common.cpp or string_common.cpp 

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
  std::string r;
  size_t begin;
  begin = s.find_first_not_of(WHITESPACE);
  if (begin == std::string::npos)
  {
    //string contains only whitespace
    r = "";
  }
  else
  {
    //do trim
    r = s.substr(begin);
  }
  return r;
}

std::string rtrim(const std::string& s)
{
  std::string r;
  size_t end;
  end = s.find_last_not_of(WHITESPACE);
  if (end == std::string::npos)
  {
    r = "";
  }
  else 
  {
    r = s.substr(0, end+1);
  }
  return r;
}

std::string trim(const std::string& s)
{
  return rtrim(ltrim(s));
}

std::string AstText::ltrim(void) const
{
  m_text = ::ltrim(m_text);
  return m_text;
}

std::string AstText::rtrim(void) const
{
  m_text = ::rtrim(m_text);
  return m_text;
}


void AstText::print() const
{
  std::string displayedString;
  displayedString = ReplaceAll(m_text, "\n", "\\n");
  displayedString = ReplaceAll(displayedString, "\t", "\\t");
  
  std::cout << "<AST_COPY_TEXT : \"" << displayedString << "\">" << std::endl;
}

void AstText::push(const AstNode* node) const
{
  (void) node;
  assert(false); // Not possible
}

void AstText::printAsm(JinjacCompiler& compiler) const
{
  StaticData* pDataContainer = compiler.getDataContainer();
  Code* pCodeContainer = compiler.getCodeContainer();
  
  if (m_text != "")
  {
    int32_t offset;
    offset = pDataContainer->insertString(m_text);
    pCodeContainer->insertStatement(OP_COPY_STRING, offset);
  }
  else
  {
    pCodeContainer->insertStatement(OP_NOP);
  }
}

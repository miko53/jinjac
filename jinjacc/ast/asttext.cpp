#include "asttext.h"
#include <assert.h>
#include <iostream>
#include "vm_opcode.h"
#include "jinjac_compiler.h"
#include "string_ext.h"

AstText::AstText(std::string& s)
{
  m_text = s;
}

AstText::~AstText()
{

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
  displayedString = ::ReplaceAll(m_text, "\n", "\\n");
  displayedString = ::ReplaceAll(displayedString, "\t", "\\t");

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

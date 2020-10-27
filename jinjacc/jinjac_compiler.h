#ifndef JINJACCOMPILER_H
#define JINJACCOMPILER_H

#include <fstream>
#include <cstdint>
#include <string>
#include "ast.h"
#include "staticdata.h"
#include "code.h"

class JinjacCompiler
{
public:
  JinjacCompiler(std::ifstream* in, std::ofstream* out);
  virtual ~JinjacCompiler();

  void doCompile(void);
  uint32_t getNoLine(void)
  {
    return m_noLine;
  }
  Ast* getRoot(void)
  {
    return m_root;
  }
  void setParsingInError(const char* s)
  {
    m_inError = true;
    m_errorDetails = std::string(s);
  }

  bool isInJinjaStatement(void)
  {
    return m_inJinjaStatement;
  }
  bool isStripBeginBlock(void)
  {
    return m_bStripBeginOfBlock;
  }
  bool isStripEndBlock(void)
  {
    return m_bStripEndOfBlock;
  }

  StaticData* getDataContainer(void)
  {
    return &m_staticData;
  }
  Code* getCodeContainer(void)
  {
    return &m_code;
  }

  void printAstTree(bool astTreeHasToPrinted)
  {
    m_isAstToBePrinted = astTreeHasToPrinted;
  }

protected:
  void parseInputFile(void);
  bool parseJinjaLang(std::string& scriptText);
  void insertAstTextNode(std::string& text);

  void buildOutputFile();
  void buildHeader();

protected:
  bool m_isAstToBePrinted;
  int32_t m_noLine;
  std::ofstream* m_outFile;
  std::ifstream* m_inFile;
  Ast*  m_root;
  bool m_inError;
  bool m_inJinjaStatement;
  std::string m_errorDetails;
  bool m_bStripBeginOfBlock;
  bool m_bStripEndOfBlock;

  StaticData m_staticData;
  Code m_code;
};

#endif // JINJACCOMPILER_H

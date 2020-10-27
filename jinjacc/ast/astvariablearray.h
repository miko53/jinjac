#ifndef ASTVARIABLEARRAY_H
#define ASTVARIABLEARRAY_H

#include "astnode.h"

class AstVariableArray : public AstNode
{
public:
  AstVariableArray(std::string& name, const AstNode* offsetExprs);
  virtual ~AstVariableArray();
    
  void print() const override;
  void push(const AstNode * node) const override;
  void printAsm(JinjacCompiler & compiler) const override;

protected:
  std::string m_name;
  const AstNode* m_offsetExprs;
};

#endif // ASTVARIABLEARRAY_H

#ifndef ASTSTRINGLITERAL_H
#define ASTSTRINGLITERAL_H

#include "astnode.h"
#include <string>

class AstStringLiteral : public AstNode
{
public:
    AstStringLiteral(std::string& s);
    virtual ~AstStringLiteral();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;

protected:
  std::string m_name;
};

#endif // ASTSTRINGLITERAL_H

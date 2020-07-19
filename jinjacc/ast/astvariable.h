#ifndef ASTVARIABLE_H
#define ASTVARIABLE_H

#include "astnode.h"
#include <string>

class AstVariable : public AstNode
{
public:
    AstVariable(std::string& m);
    virtual ~AstVariable();
    virtual void print() const override;
    virtual void push(const AstNode* node) const override;
    void printAsm(JinjacCompiler & compiler) const override;

protected:
    std::string m_name;
};

#endif // ASTVARIABLE_H

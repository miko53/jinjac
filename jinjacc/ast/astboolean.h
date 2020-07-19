#ifndef ASTBOOLEAN_H
#define ASTBOOLEAN_H

#include "astnode.h"

/**
 * @todo write docs
 */
class AstBoolean : public AstNode
{
public:
    AstBoolean(bool b);
    virtual ~AstBoolean();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;

protected:
  bool m_value;
};

#endif // ASTBOOLEAN_H

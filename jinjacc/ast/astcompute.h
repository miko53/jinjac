#ifndef ASTCOMPUTE_H
#define ASTCOMPUTE_H

#include "astnode.h"

class AstCompute : public AstNode
{
public:
    AstCompute(const AstNode* left, const AstNode* right, char operation);
    virtual ~AstCompute();
    
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;

protected:
  const AstNode* m_left;
  const AstNode* m_right;
  char m_operation;
};

#endif // ASTCOMPUTE_H

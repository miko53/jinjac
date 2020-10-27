#ifndef ASTCONDITION_H
#define ASTCONDITION_H

#include "astnode.h"

class AstCondition : public AstNode
{
public:
    typedef enum
    {
      AST_EQUAL,
      AST_DIFFERENT,
      AST_HIGH_AND_EQUAL_THAN,
      AST_HIGH_THAN,
      AST_LOWER_AND_EQUAL_THAN,
      AST_LOWER_THAN,
      AST_OR,
      AST_AND,
      AST_NOT,
      AST_IS
    } ConditionType;
    
    AstCondition(const AstNode* left, const AstNode* right, ConditionType operation);
    virtual ~AstCondition();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;
    
protected:
    ConditionType m_operation;
    const AstNode* m_left;
    const AstNode* m_right;
};

#endif // ASTCONDITION_H

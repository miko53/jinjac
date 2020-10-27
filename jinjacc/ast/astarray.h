#ifndef ASTARRAY_H
#define ASTARRAY_H

#include "astnode.h"
#include <vector>

class AstArray : public AstNode
{
public:
    AstArray(const AstNode* firstItems);
    virtual ~AstArray();
    virtual void print() const override;
    virtual void push(const AstNode * node) const override;
    virtual void printAsm(JinjacCompiler & compiler) const override;
    void setTuple(bool isTuple) const { m_isTuple = isTuple; }
    
protected:
    mutable std::vector<const AstNode* > m_items;
    mutable bool m_isTuple;
};

#endif // ASTARRAY_H

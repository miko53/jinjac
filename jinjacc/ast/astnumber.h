#ifndef ASTNUMBER_H
#define ASTNUMBER_H

#include "astnode.h"

/**
 * @todo write docs
 */
class AstNumber : public AstNode
{
public:
    AstNumber(int32_t number);
    AstNumber(double number);
    AstNumber();
    virtual ~AstNumber();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;
    
    AstNumber operator+(const AstNumber& r) const;
    AstNumber operator-(const AstNumber& r) const;
    AstNumber operator*(const AstNumber& r) const;
    AstNumber operator/(const AstNumber& r) const;
    
protected:
    int32_t getOperationType(const AstNumber& r) const;
    
protected:
  typedef enum { DOUBLE, INTEGER } type;
  typedef union { double dlb; int32_t integer; } variant;
  variant m_value;
  type m_type;
    
};

#endif // ASTNUMBER_H

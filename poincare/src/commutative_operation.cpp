#include <poincare/commutative_operation.h>
#include <poincare/float.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

CommutativeOperation::CommutativeOperation(Expression ** operands,
    int numberOfOperands,
    bool cloneOperands) : m_numberOfOperands(numberOfOperands) {
  assert(operands != nullptr);
  assert(numberOfOperands >= 2);
  m_operands = (Expression **)malloc(numberOfOperands*sizeof(Expression *));
  for (int i=0; i<numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

CommutativeOperation::~CommutativeOperation() {
  for (int i=0; i<m_numberOfOperands; i++) {
    delete m_operands[i];
  }
  free(m_operands);
}

int CommutativeOperation::numberOfOperands() const {
  return m_numberOfOperands;
}

const Expression * CommutativeOperation::operand(int i) const {
  assert(i >= 0);
  assert(i < m_numberOfOperands);
  return m_operands[i];
}

Expression * CommutativeOperation::clone() const {
  return this->cloneWithDifferentOperands(m_operands, m_numberOfOperands, true);
}

float CommutativeOperation::approximate(Context& context) const {
  float result = m_operands[0]->approximate(context);
  for (int i=1; i<m_numberOfOperands; i++) {
    float next = m_operands[i]->approximate(context);
    result = this->operateApproximatevelyOn(result, next);
  }
  return result;
}

Expression * CommutativeOperation::createEvaluation(Context& context) const {
  Expression * result = m_operands[0]->createEvaluation(context);
  for (int i=1; i<m_numberOfOperands; i++) {
    Expression * next = m_operands[i]->createEvaluation(context);
    Expression * newResult = this->createEvaluationOn(result, next, context);
    delete result;
    result = newResult;
    delete next;
  }
  assert(result == nullptr || result->type() == Expression::Type::Float || result->type() == Expression::Type::Matrix || result->type() == Expression::Type::List);
  return result;
}

ExpressionLayout * CommutativeOperation::createLayout() const {
  int number_of_children = 2*m_numberOfOperands-1;
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(number_of_children*sizeof(ExpressionLayout *));
  char string[2] = {operatorChar(), '\0'};
  children_layouts[0] = m_operands[0]->createLayout();
  for (int i=1; i<m_numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(string, 1);
    children_layouts[2*i] = m_operands[i]->createLayout();
  }
  return new HorizontalLayout(children_layouts, number_of_children);
}

bool CommutativeOperation::isCommutative() const {
  return true;
}
